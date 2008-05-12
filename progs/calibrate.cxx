#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>

#include <TooN/helpers.h>
#include <TooN/wls.h>
#include <TooN/Cholesky.h>
#include <TooN/se3.h>

#include <cvd/camera.h>
#include <cvd/convolution.h>
#include <cvd/videosource.h>
#include <cvd/gl_helpers.h>
#include <cvd/vision.h>
#include <cvd/draw.h>
#include <cvd/image_io.h>
#include <cvd/image_interpolate.h>
#include <cvd/videodisplay.h>
#include <cvd/random.h>
#include <cvd/timer.h>
#include <cvd/colourspaces.h>
#include <cvd/colourspace_convert.h>

using namespace std;
using namespace TooN;
using namespace CVD;

#include <X11/keysym.h>
#include <X11/Xlib.h>

#ifdef CVD_HAVE_QTBUFFER
typedef vuy422 CAMERA_PIXEL;
#else
typedef byte CAMERA_PIXEL;
#endif

VideoBuffer<CAMERA_PIXEL>* videoBuffer=0;

// global configuration variables, can be set via command line options
#ifdef CVD_HAVE_QTBUFFER
string videoDevice = "qt://0";
#else
string videoDevice = "v4l2:///dev/video0";
#endif
Vector<6> cameraParameters = (make_Vector, 1000,  1000,  320,  240,  0,  0);
int bottomLeftWhite = 1;
int gridx = 11;
int gridy = 7;
double cellSize = 0.02;
double interval = 0.5;
string input, output;
int generateErrorImage = 0;

template <class T>
std::ostream& operator << (std::ostream &os, Image<T> im)
{
  ImageRef size = im.size();
  for (int i = 0; i < size.x; i++)
    {
      for (int j = 0; j < size.y; j++)
        {
          ImageRef temp (i, j);
          std::cout << im[temp] << "     ";
        }
      std::cout << std::endl;
    }

  return os;
}

template <class A1, class A2>
void makeProjectionParameterJacobian(const FixedVector<3,A1>& x_se3, FixedMatrix<2,6,A2>& J_pose)
{
  double z_inv = 1.0/x_se3[2];
  double z_inv_sq = z_inv*z_inv;
  double cross = x_se3[0]*x_se3[1] * z_inv_sq;
  J_pose[0][0] = z_inv; J_pose[0][1] = 0; J_pose[0][2] = -x_se3[0]*z_inv_sq;
  J_pose[0][3] = -cross;  J_pose[0][4] = 1 + x_se3[0]*x_se3[0]*z_inv_sq;  J_pose[0][5] = -x_se3[1]*z_inv;
  J_pose[1][0] = 0; J_pose[1][1] =  z_inv; J_pose[1][2] = -x_se3[1]*z_inv_sq;
  J_pose[1][3] = -1 - x_se3[1]*x_se3[1]*z_inv_sq; J_pose[1][4] =  cross;  J_pose[1][5] =  x_se3[0]*z_inv;
}

void drawGrid (vector<Vector<2> > grid, int cols, int rows)
{
  glBegin (GL_LINES);
  for (unsigned int i = 0; i < grid.size(); i++)
    {
      if (((i+1) % (cols+1)))
        {
          //Not last column, draw a horiz line
          glVertex(grid[i]);
          glVertex(grid[i+1]);
        }

      if (i < (unsigned int)rows * (cols+1))
        {
          //Not top row, draw a vert line
          glVertex(grid[i]);
          glVertex(grid[i+cols+1]);
        }
    }
  glEnd ();
}

void drawCross(Vector<2> pos, double size)
{
  glBegin (GL_LINES);
  glVertex2f (pos[0] - size / 2, pos[1]);
  glVertex2f (pos[0] + size / 2, pos[1]);
  glVertex2f (pos[0] , pos[1] - size / 2);
  glVertex2f (pos[0] , pos[1] + size / 2);
  glEnd ();
}

void drawCross(ImageRef pos, int size)
{
  glBegin (GL_LINES);
  glVertex2i (pos.x - size / 2, pos.y);
  glVertex2i (pos.x + size / 2, pos.y);
  glVertex2i (pos.x , pos.y - size / 2);
  glVertex2i (pos.x , pos.y + size / 2);
  glEnd ();
}

template <class CamModel, class P>
CVD::SE3 findSE3(const CVD::SE3& start, const vector<Vector<3> >& x, const vector<pair<size_t,P> >& p, const CamModel& camModel, Matrix<6>& Cinv, double noise)
{
  Matrix<2> Rinv;
  Identity(Rinv, camModel.get_parameters()[0] * camModel.get_parameters()[0]/noise);

  CVD::SE3 bestSE3;
  double bestError = numeric_limits<double>::max();
  vector<Vector<2> > up(p.size());
  for (size_t i=0; i<p.size(); i++)
    up[i] = camModel.unproject(p[i].second);
  CVD::SE3 se3;
  se3 = start;
  int iter = 0;
  while (++iter < 20)
    {
      WLS<6> wls;
      wls.clear();
      wls.add_prior(0.01);
      for (unsigned int i=0; i<p.size(); i++)
        {
          Vector<3> wx = se3*x[p[i].first];
          Vector<2> camFrame = project(wx);
          Matrix<2,6> J;
          makeProjectionParameterJacobian(wx, J);
          Vector<2> delta = up[i]-camFrame;
          wls.add_df(delta, J.T(), Rinv);
        }
      wls.compute();
      Vector<6> change = wls.get_mu();
      se3 = CVD::SE3::exp(change)*se3;
      double error = 0;
      for (size_t i=0; i<p.size(); i++)
        {
          Vector<3> cx = se3*x[p[i].first];
          Vector<2> pp = project(cx);
          error += (pp-up[i])*(pp-up[i]);
        }
      if (error < bestError)
        {
          bestError = error;
          bestSE3 = se3;
          Cinv = wls.get_C_inv();
        }
      else
        {
          //std::cerr << "findSE3 is diverging!\n";
          break;
        }
    }
  return bestSE3;
}

template <class CamModel, class P>
void findParams(const CVD::SE3& pose, const vector<Vector<3> >& x, const vector<pair<size_t,P> >& p, CamModel& camModel, double noise)
{
  static const int np = CamModel::num_parameters;
  Vector<np> bestParams = camModel.get_parameters();
  double bestError = numeric_limits<double>::max();
  Matrix<2> Rinv;
  Identity(Rinv, 1.0/noise);
  int iter = 0;
  while (++iter < 20)
    {
      WLS<6> wls;
      wls.clear();
      double preError = 0;
      for (unsigned int i=0; i<p.size(); i++)
        {
          Vector<3> wx = pose*x[p[i].first];
          Vector<2> plane = project(wx);
          Vector<2> im = camModel.project(plane);
          Vector<2> delta = p[i].second - im;
          preError += delta*delta;
          wls.add_df(delta, camModel.get_parameter_derivs(), Rinv);
        }
      if (preError < bestError)
        bestError = preError;
      wls.compute();
      camModel.get_parameters() += wls.get_mu();
      double error = 0;
      for (size_t i=0; i<p.size(); i++)
        {
          Vector<3> cx = pose*x[p[i].first];
          Vector<2> im = camModel.project(project(cx));
          Vector<2> diff = im - p[i].second;
          error += diff*diff;
        }
      if (error < bestError)
        {
          bestError = error;
          bestParams = camModel.get_parameters();
        }
    }
  camModel.get_parameters() = bestParams;
}

void getOptions(int argc, char* argv[])
{
  for (int i = 1; i<argc; i++){
      string arg = argv[i];
      if (arg == "-o") {
          output = argv[++i];
      } else if (arg == "-i") {
          input = argv[++i];
      } else if (arg == "-d") {
          videoDevice = argv[++i];
      } else if (arg == "-x") {
          gridx = atoi(argv[++i]);
      } else if (arg == "-y") {
          gridy = atoi(argv[++i]);
      } else if (arg == "-s") {
          cellSize = atof(argv[++i]);
      } else if (arg == "-t") {
          interval = atof(argv[++i]);
      } else if (arg == "-c") {
          istringstream sin(argv[++i]);
          sin >> cameraParameters;
      } else if (arg == "-e") {
          generateErrorImage = 1;
      } else {
        cout << argv[0] << " [options]\n\n"
        "Move virtual grid over real grid until it snaps on.\n"
        "Record a number of frames ~100.\n"
        "Press SPACE to calculate camera parameters.\n\n"
        "A calibration grid for the default parameters is in libcvd/doc/cameracalib2cm.pdf\n\n"
        "flag  description\t\t\t\tdefault\n"
        "  -d  device to open, CVD video source URL\tv4l2:///dev/video0\n"
        "  -x  grid dimension in x (width) direction\t11\n"
        "  -y  grid dimension in y (height) direction\t7\n"
        "  -s  grid cell size\t\t\t\t0.02 m\n"
        "  -t  interval between captured frames\t\t0.5 s\n"
        "  -c  initial camera parameters\t\t\t1000 1000 320 240 0 0\n"
        "  -e  generate image showing errors per pixel\n";
        exit(0);
      }
    }
  if (input.length() != 0)
    return;
  try {
        cout << "opening " << videoDevice << endl;
        videoBuffer = open_video_source<CAMERA_PIXEL>(videoDevice);
  }
  catch (CVD::Exceptions::All& e) {
      cerr << e.what << endl;
      exit(1);
  }
}

vector<Vector<2> > makeGrid(int gx, int gy, double cellSize)
{
  vector<Vector<2> > grid;
  Vector<2> center = (make_Vector, gx, gy);
  center *= cellSize/2.0;
  for (int y=0; y<=gy; y++)
    {
      for (int x=0; x<=gx; x++)
        {
          grid.push_back(Vector<2>((make_Vector,x,y))*cellSize - center);
        }
    }
  return grid;
}

template <class CM>
void drawPoints(const vector<Vector<2> >& points, const SE3& pose, const CM& cm)
{
  glColor3f(0,1,0);
  for (size_t i=0; i<points.size(); i++)
    {
      Vector<3> x = unproject(points[i]);
      Vector<2> plane = project(pose * x);
      Vector<2> im = cm.project(plane);
      drawCross(im, 6);
    }
}

struct MeasurementSet
  {
    vector<Vector<2> > im;
    vector<Vector<3> > world;
  };

template <class CM>
double getReprojectionError(const vector<MeasurementSet>& ms, const vector<SE3>& poses, CM& cm)
{
  double error = 0;
  for (size_t i=0; i<ms.size(); i++)
    {
      for (size_t j=0; j<ms[i].im.size(); j++)
        {
          Vector<3> camFrame = poses[i] * ms[i].world[j];
          Vector<2> im = cm.project(project(camFrame));
          Vector<2> v = ms[i].im[j] - im;
          error += v*v;
        }
    }
  return error;
}

template <class CM>
pair<double, double> getReprojectionError(const vector<MeasurementSet>& ms, const vector<SE3>& poses, CM& cm,
                                          vector<pair<Vector<2>,Vector<2> > >& v)
{
  double error = 0, maxError = 0;
  for (size_t i=0; i<ms.size(); i++)
    {
      for (size_t j=0; j<ms[i].im.size(); j++)
        {
          Vector<3> camFrame = poses[i] * ms[i].world[j];
          Vector<2> im = cm.project(project(camFrame));
          Vector<2> err = ms[i].im[j] - im;
          v.push_back(make_pair(im, err));
          double thisError = err*err;
          maxError = std::max(maxError, thisError);
          error += thisError;
        }
    }
  return make_pair(error, maxError);
}

template <class CM>
double getReprojectionError(const vector<MeasurementSet>& ms, const vector<vector<Vector<2> > >& plane, CM& cm)
{
  assert(plane.size() == ms.size());
  double error = 0;
  for (size_t i=0; i<ms.size(); i++)
    {
      for (size_t j=0; j<plane[i].size(); j++)
        {
          Vector<2> im = cm.project(plane[i][j]);
          Vector<2> v = ms[i].im[j] - im;
          error += v*v;
        }
    }
  return error;
}

template <class CM>
void improve(vector<MeasurementSet>& ms, vector<SE3>& pose, CM& cm, double epsilon)
{
  Vector<CM::num_parameters> J_param;
  zero(J_param);
  for (size_t i=0; i<ms.size(); i++)
    {
      Vector<6> J_pose_sum;
      Zero(J_pose_sum);
      for (size_t j=0; j<ms[i].im.size(); j++)
        {
          Vector<3> camFrame = pose[i] * ms[i].world[j];
          Vector<2> im = cm.project(project(camFrame));
          Vector<2> v = ms[i].im[j] - im;
          Matrix<2,6> J_pose;
          makeProjectionParameterJacobian(camFrame, /*pose[i],*/ J_pose);
          J_pose_sum += (cm.get_derivative() * J_pose).T() * v;
          J_param += cm.get_parameter_derivs(v);
        }
      pose[i] = SE3::exp(J_pose_sum * epsilon) * pose[i];
    }
  cm.get_parameters() += epsilon * J_param;
}

template <class CM>
void improvePoses(const vector<MeasurementSet>& ms, vector<SE3>& pose, const CM& cm)
{
  for (size_t i=0; i<ms.size(); i++)
    {
      WLS<6> wls;
      wls.clear();
      wls.add_prior(1e-3);
      for (size_t j=0; j<ms[i].im.size(); j++)
        {
          Vector<3> camFrame = pose[i] * ms[i].world[j];
          Vector<2> im = cm.project(project(camFrame));
          Vector<2> v = ms[i].im[j] - im;
          Matrix<2,6> J_pose;
          makeProjectionParameterJacobian(camFrame, /*pose[i],*/ J_pose);
          J_pose = cm.get_derivative() * J_pose;
          wls.add_df(v[0], J_pose[0]);
          wls.add_df(v[1], J_pose[1]);
        }
      wls.compute();
      pose[i] = SE3::exp(wls.get_mu()) * pose[i];
    }
}

template <class CM>
void improveParams(const vector<MeasurementSet>& ms, const vector<SE3>& pose, CM& cm)
{
  WLS<CM::num_parameters> wls;
  wls.clear();
  wls.add_prior(1e-3);
  for (size_t i=0; i<ms.size(); i++)
    {
      for (size_t j=0; j<ms[i].im.size(); j++)
        {
          Vector<3> camFrame = pose[i] * ms[i].world[j];
          Vector<2> im = cm.project(project(camFrame));
          Vector<2> v = ms[i].im[j] - im;
          Matrix<2,CM::num_parameters> J_param = cm.get_parameter_derivs().T();
          wls.add_df(v[0], J_param[0]);
          wls.add_df(v[1], J_param[1]);
        }
    }
  wls.compute();
  cm.get_parameters() += wls.get_mu();
}

template <class CM>
void improveLM(vector<MeasurementSet>& ms, vector<SE3>& pose, CM& cm, double lambda)
{
  Matrix<> JTJ(CM::num_parameters+ms.size()*6,CM::num_parameters+ms.size()*6);
  Vector<> JTe(JTJ.num_rows());
  Zero(JTJ);
  Zero(JTe);
  for (size_t i=0; i<ms.size(); i++)
    {
      Matrix<6> poseBlock;
      Matrix<CM::num_parameters> paramBlock;
      Matrix<CM::num_parameters, 6> offDiag;
      Zero(poseBlock);
      Zero(paramBlock);
      Zero(offDiag);
      for (size_t j=0; j<ms[i].im.size(); j++)
        {
          Vector<3> camFrame = pose[i] * ms[i].world[j];
          Vector<2> im = cm.project(project(camFrame));
          Vector<2> v = ms[i].im[j] - im;
          Matrix<2,6> J_pose;
          makeProjectionParameterJacobian(camFrame, /*pose[i],*/ J_pose);
          J_pose = cm.get_derivative() * J_pose;
          Matrix<2,CM::num_parameters> J_param = cm.get_parameter_derivs().T();
          poseBlock += J_pose.T()*J_pose;
          paramBlock += J_param.T() * J_param;
          offDiag += J_param.T() * J_pose;
          JTe.slice(CM::num_parameters + i*6, 6) = JTe.slice(CM::num_parameters + i*6, 6) + J_pose.T() * v;
          JTe.slice(0,CM::num_parameters) = JTe.slice(0,CM::num_parameters) + J_param.T() * v;
        }
      JTJ.slice(CM::num_parameters + i*6, CM::num_parameters + i*6, 6,6) = poseBlock;
      JTJ.slice(0,0,CM::num_parameters, CM::num_parameters) = JTJ.slice(0,0,CM::num_parameters, CM::num_parameters) + paramBlock;
      JTJ.slice(0,CM::num_parameters + i*6, CM::num_parameters, 6) = offDiag;
      JTJ.slice(CM::num_parameters + i*6,0, 6, CM::num_parameters) = offDiag.T();
    }
  for (int i=0; i<JTJ.num_rows(); i++)
    JTJ[i][i] += lambda;
  Vector<> delta = Cholesky<>(JTJ).backsub(JTe);
  cm.get_parameters() += delta.template slice<0,CM::num_parameters>();
  for (size_t i=0; i<pose.size(); i++)
    {
      pose[i] = SE3::exp(delta.slice(CM::num_parameters+i*6, 6)) * pose[i];
    }
}

template <class CM>
void getUncertainty(const vector<MeasurementSet>& ms, const vector<SE3>& pose, const CM& cm, Matrix<CM::num_parameters>& C)
{
  Matrix<> JTJ(CM::num_parameters+ms.size()*6,CM::num_parameters+ms.size()*6);
  Zero(JTJ);
  for (size_t i=0; i<ms.size(); i++)
    {
      Matrix<6> poseBlock;
      Matrix<CM::num_parameters> paramBlock;
      Matrix<CM::num_parameters, 6> offDiag;
      Zero(poseBlock);
      Zero(paramBlock);
      Zero(offDiag);
      for (size_t j=0; j<ms[i].im.size(); j++)
        {
          Vector<3> camFrame = pose[i] * ms[i].world[j];
          Vector<2> im = cm.project(project(camFrame));
          Vector<2> v = ms[i].im[j] - im;
          Matrix<2,6> J_pose;
          makeProjectionParameterJacobian(camFrame, /*pose[i],*/ J_pose);
          J_pose = cm.get_derivative() * J_pose;
          Matrix<2,CM::num_parameters> J_param = cm.get_parameter_derivs().T();
          poseBlock += J_pose.T()*J_pose;
          paramBlock += J_param.T() * J_param;
          offDiag += J_param.T() * J_pose;
        }
      JTJ.slice(CM::num_parameters + i*6, CM::num_parameters + i*6, 6,6) = poseBlock;
      JTJ.slice(0,0,CM::num_parameters, CM::num_parameters) = JTJ.slice(0,0,CM::num_parameters, CM::num_parameters) + paramBlock;
      JTJ.slice(0,CM::num_parameters + i*6, CM::num_parameters, 6) = offDiag;
      JTJ.slice(CM::num_parameters + i*6,0, 6, CM::num_parameters) = offDiag.T();
    }
  Matrix<> Cov = LU<>(JTJ).get_inverse();
  C = Cov.template slice<0,0,CM::num_parameters, CM::num_parameters>();
}

template <int N>
Vector<N> sampleUnitSphere()
{
  Vector<N> s;
  do
    {
      for (int i=0; i<N; i++)
        s[i] = drand48()*2 - 1.0;
    }
  while (s*s > 1);
  return s;
}

void saveData(ostream& out, ImageRef size, double factor, const Vector<6>& parameters, const vector<MeasurementSet>& ms, const vector<SE3>& poses)
{
  out << size.x << " \t" << size.y << endl;
  out << factor << endl;
  out << parameters << endl;
  out << ms.size() << endl;
  for (size_t i=0; i<ms.size(); i++)
    {
      out << poses[i].ln() << endl;
      out << ms[i].im.size() << endl;
      for (size_t j=0; j<ms[i].im.size(); j++)
        {
          out << ms[i].im[j] << endl;
          out << ms[i].world[j] << endl;
        }
    }
}

void loadData(istream& in, ImageRef& size, double& factor, Vector<6>& params, vector<MeasurementSet>& ms, vector<SE3>& poses)
{
  size_t views;
  in >> size.x >> size.y >> factor >> params >> views;
  ms.resize(views);
  poses.resize(views);
  for (size_t i=0; i<views; i++)
    {
      Vector<6> ln;
      in >> ln;
      poses[i] = SE3::exp(ln);
      size_t points;
      in >> points;
      ms[i].im.resize(points);
      ms[i].world.resize(points);
      for (size_t j=0; j<points; j++)
        in >> ms[i].im[j] >> ms[i].world[j];
    }
}

inline Vector<2> imagePoint(const Vector<2>& inPoint, const SE3& pose, const Camera::Quintic& cm, const double& factor)
{
  Vector<3> point3D = unproject(inPoint);
  Vector<2> plane = project(pose*point3D);
  Vector<2> im = cm.project(plane) / factor;

  return im;
}

inline float imageVal(image_interpolate<Interpolate::Bilinear, float> &imgInter, const Vector<2>& inPoint, const SE3& pose,
                      const Camera::Quintic& cm, const double& factor, const bool& boundsCheck)
{
  Vector<2> imageVec = imagePoint(inPoint, pose, cm, factor);
  if(!boundsCheck) //Slight speed-up
    return imgInter[imageVec];

  if (imgInter.in_image(imageVec))
    return imgInter[imageVec];
  else
    return -1;

}

//l,t,r,b
inline bool inSquareX(const Vector<2>& inPoint, const Vector<4>& calibSquare)
{
  if(inPoint[0] < calibSquare[0])
    return false;
  if(inPoint[0] > calibSquare[2])
    return false;

  return true;
}
//l,t,r,b
inline bool inSquareY(const Vector<2>& inPoint, const Vector<4>& calibSquare)
{
  if(inPoint[1] > calibSquare[1])
    return false;
  if(inPoint[1] < calibSquare[3])
    return false;

  return true;
}

inline float minMargin(const Vector<2>& imagePoint, const Vector<2>& minCoord, const Vector<2>& maxCoord)
{
  return std::min(imagePoint[0]-minCoord[0],
                  std::min(imagePoint[1]-minCoord[1],
                           std::min(maxCoord[0] - imagePoint[0], maxCoord[1] - imagePoint[1])));
}

inline float minMarginSquare(const Vector<2>& inPoint, image_interpolate<Interpolate::Bilinear, float>& imgInter, const SE3& pose,
                             const Camera::Quintic& cm, double factor, float cellSize)
{
  Vector<2> posVec = (make_Vector, cellSize/2, cellSize/2);
  Vector<2> negVec = (make_Vector, cellSize/2, -cellSize/2);

  Vector<2> minCoord = imgInter.min();
  Vector<2> maxCoord = imgInter.max();

  float minVal = minMargin(imagePoint(inPoint, pose, cm, factor), minCoord, maxCoord);
  minVal = std::min(minVal, minMargin(imagePoint(inPoint - negVec, pose, cm, factor), minCoord, maxCoord)); //top-left
  minVal = std::min(minVal, minMargin(imagePoint(inPoint + posVec, pose, cm, factor), minCoord, maxCoord)); //top-right
  minVal = std::min(minVal, minMargin(imagePoint(inPoint - posVec, pose, cm, factor), minCoord, maxCoord)); //bottom-left
  minVal = std::min(minVal, minMargin(imagePoint(inPoint + negVec, pose, cm, factor), minCoord, maxCoord)); //bottom-right

  return minVal;
}

bool sanityCheck(const Vector<2>& inPoint, image_interpolate<Interpolate::Bilinear, float>& imgInter, const SE3& pose,
                 const Camera::Quintic& cm, double factor, bool blWhite, float cellSize)
{
  Vector<2> posVec = (make_Vector, cellSize/2, cellSize/2);
  Vector<2> negVec = (make_Vector, cellSize/2, -cellSize/2);

  // Don't need to bounds check these as it's done after the minMarginSquare > 0 check
  float midVal = imageVal(imgInter, inPoint, pose, cm, factor, false);
  float tlVal = imageVal(imgInter, inPoint - negVec, pose, cm, factor, false);
  float trVal = imageVal(imgInter, inPoint + posVec, pose, cm, factor, false);
  float blVal = imageVal(imgInter, inPoint - posVec, pose, cm, factor, false);
  float brVal = imageVal(imgInter, inPoint + negVec, pose, cm, factor, false);

  if(blWhite)
    {
      if(midVal - tlVal < 0.05)
        return false;
      if(trVal - midVal < 0.05)
        return false;
      if(blVal - midVal < 0.05)
        return false;
      if(midVal - brVal < 0.05)
        return false;
    }
  else
    {
      if(tlVal - midVal < 0.05)
        return false;
      if(midVal - trVal < 0.05)
        return false;
      if(midVal - blVal < 0.05)
        return false;
      if(brVal - midVal < 0.05)
        return false;
    }

  return true;
}

bool findInitialIntersectionEstimate(image_interpolate<Interpolate::Bilinear, float> &imgInter, Vector<2>& initialPoint,
                                     const SE3& pose, const Camera::Quintic& cm, double factor,
                                     bool boundsCheck, const Vector<4>& likelySquare, double cellSize)
{
  bool looksOK = true;

  //very roughly find a sensible starting place
  Vector<2> testPoint = (make_Vector, likelySquare[0], likelySquare[1]);
  float startPx = imageVal(imgInter, testPoint, pose, cm, factor, boundsCheck);
  while(looksOK && fabs(imageVal(imgInter, testPoint, pose, cm, factor, boundsCheck) - startPx) < 0.1)
    {
      testPoint[0] += cellSize/10;
      if(testPoint[0] > likelySquare[2])
        looksOK = false;
    }
  if(looksOK)
    initialPoint[0] = testPoint[0];

  testPoint[0] = likelySquare[0];
  while(looksOK && fabs(imageVal(imgInter, testPoint, pose, cm, factor, boundsCheck) - startPx) < 0.1)
    {
      testPoint[1] -= cellSize/10;
      if(testPoint[1] < likelySquare[3])
        looksOK = false;
    }
  if(looksOK)
    initialPoint[1] = testPoint[1];

  return looksOK;
}

bool optimiseIntersection(image_interpolate<Interpolate::Bilinear, float> &imgInter, Vector<2>& inPoint,
                          const SE3& pose, const Camera::Quintic& cm, double factor, bool boundsCheck,
                          const Vector<4>& likelySquare, double cellSize, bool blWhite)
{
  Vector<2> largeX = (make_Vector, 0.004, 0);
  Vector<2> largeY = (make_Vector, 0, 0.004);
  Vector<2> smallX = (make_Vector, cellSize/10, 0);
  Vector<2> smallY = (make_Vector, 0, cellSize/10);

  float aboveVal = imageVal(imgInter, inPoint + largeY, pose, cm, factor, boundsCheck);
  float belowVal = imageVal(imgInter, inPoint - largeY, pose, cm, factor, boundsCheck);
  float leftVal = imageVal(imgInter, inPoint - largeX, pose, cm, factor, boundsCheck);
  float rightVal = imageVal(imgInter, inPoint + largeX, pose, cm, factor, boundsCheck);

  while(fabs(aboveVal - belowVal) > 0.2 || fabs(leftVal - rightVal) > 0.2)
    {
      //reduce step size
      smallY = smallY/2;
      smallX = smallX/2;

      while(belowVal < aboveVal)
        {
          //lighter above - move left if bottom-left white, otherwise right
          blWhite ? inPoint -= smallX : inPoint += smallX;
          if(!inSquareX(inPoint, likelySquare))
            break;
          aboveVal = imageVal(imgInter, inPoint + largeY, pose, cm, factor, boundsCheck);
          belowVal = imageVal(imgInter, inPoint - largeY, pose, cm, factor, boundsCheck);
        }
      while(belowVal > aboveVal)
        {
          //darker above - move right if bottom-left white, otherwise left
          blWhite ? inPoint += smallX : inPoint -= smallX;
          if(!inSquareX(inPoint, likelySquare))
            break;
          aboveVal = imageVal(imgInter, inPoint + largeY, pose, cm, factor, boundsCheck);
          belowVal = imageVal(imgInter, inPoint - largeY, pose, cm, factor, boundsCheck);
        }

      while(rightVal > leftVal)
        {
          //darker on left - move down if bottom-left white, otherwise up
          blWhite ? inPoint -= smallY : inPoint += smallY;
          if(!inSquareY(inPoint, likelySquare))
            break;
          leftVal = imageVal(imgInter, inPoint - largeX, pose, cm, factor, boundsCheck);
          rightVal = imageVal(imgInter, inPoint + largeX, pose, cm, factor, boundsCheck);
        }
      while(rightVal < leftVal)
        {
          //lighter on left - move up if bottom-left white, otherwise down
          blWhite ? inPoint += smallY : inPoint -= smallY;
          if(!inSquareY(inPoint, likelySquare))
            break;
          leftVal = imageVal(imgInter, inPoint - largeX, pose, cm, factor, boundsCheck);
          rightVal = imageVal(imgInter, inPoint + largeX, pose, cm, factor, boundsCheck);
        }

      if(!(inSquareX(inPoint, likelySquare) && inSquareY(inPoint, likelySquare)))
        break;

      if(smallX[0] < 0.000002)
        break;  //prevents infinite loop with certain patterns
    }

  return (fabs(aboveVal - belowVal) < 1 && fabs(leftVal - rightVal) < 1);
}

int main(int argc, char* argv[])
{
  getOptions(argc, argv);

  VideoDisplay disp (0.0, 0.0, 640.0, 480.0);
  Camera::Quintic cameraModel;
  double factor=1.0;
  vector<MeasurementSet> measurementSets;
  vector<SE3> poses;
  ImageRef imageSize;

  XEvent e;
  disp.select_events(KeyPressMask);
  bool doParams = false;
  int stage = 2; // 2 is init, 1 record, 0 done

  string titlePrefix = "Calibrate: Align grid ([ and ] to resize)";
  disp.set_title(titlePrefix);

  double curr = timer.get_time();
  srand48(static_cast<long int>(curr));
  srand(static_cast<unsigned int>(curr));
  if (!input.length())
    {
      imageSize = videoBuffer->size();

      cameraModel.get_parameters() = cameraParameters;
      factor = 1.0/std::max(imageSize.x, imageSize.y);
      cameraModel.get_parameters().slice<0,4>() *= factor;

      vector<Vector<2> > grid = makeGrid(gridx, gridy, cellSize);
      vector<Vector<3> > grid3d(grid.size());
      for (size_t i=0; i<grid.size(); i++)
        grid3d[i] = unproject(grid[i]);

      SE3 pose;
      glPixelStorei(GL_UNPACK_ALIGNMENT,1);
      glDrawBuffer(GL_BACK);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

      double lastMeasure = timer.get_time();
      bool prevFrameTracked = false;
      while (stage)
        {
          // handle X events
          while(disp.pending())
            {
              disp.get_event(&e);
              if(e.type == KeyPress)
                {
                  KeySym k;
                  XLookupString(&e.xkey, 0, 0, &k, 0);
                  if(k == XK_Home)
                    {
                      pose = SE3::exp((make_Vector, 0, 0, 0, 0, 0, 0));
                    }
                  else if(k == XK_bracketleft)
                    {
                      pose.get_translation()[2] += 0.02;
                    }
                  else if(k == XK_bracketright)
                    {
                      pose.get_translation()[2] -= 0.02;
                    }
                  else if(k == XK_Escape || k == XK_q || k == XK_Q)
                    {
                      return 0;
                    }
                  else if(k == XK_space)
                    {
                      if(stage == 1)
                        {
                          stage = 0;
                          titlePrefix = "Calibrate: Calculating Camera Parameters";
                          disp.set_title(titlePrefix);
                        }
                    }
                  else if(k == XK_p)
                    {
                      doParams = !doParams;
                      if(stage == 1)
					  {
                        if(doParams)
                          titlePrefix = "Calibrate: Tracking Pose and Internal Params";
                        else
                          titlePrefix = "Calibrate: Tracking Pose";
					  }
                    }
                }
            }

          VideoFrame<CAMERA_PIXEL>* vframe = videoBuffer->get_frame();
          while(videoBuffer->frame_pending())
            {
              videoBuffer->put_frame(vframe);
              vframe = videoBuffer->get_frame();
            }
          Image<byte> temp = convert_image(*vframe);
          Image<float> gray = convert_image(temp);
          videoBuffer->put_frame(vframe);

          glDisable(GL_BLEND);
          glEnable(GL_TEXTURE_RECTANGLE_NV);
          glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
          glTexParameterf( GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
          glTexParameterf( GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
          glPixelStorei(GL_UNPACK_ALIGNMENT,1);
          glTexImage2D( temp, 0, GL_TEXTURE_RECTANGLE_NV);
          glBegin(GL_QUADS);
              glTexCoord2i(0, 0);
              glVertex2i(0,0);
              glTexCoord2i(temp.size().x, 0);
              glVertex2i(640,0);
              glTexCoord2i(temp.size().x,temp.size().y);
              glVertex2i(640,480);
              glTexCoord2i(0, temp.size().y);
              glVertex2i(0, 480);
          glEnd ();
          glDisable(GL_TEXTURE_RECTANGLE_NV);
          glEnable(GL_BLEND);

          //this is the bit that does the calibrating
          vector<pair<size_t, Vector<2> > > measurements;
          vector<Vector<2> > failedPoints;

          //smooth and prepare image for interpolation
          convolveGaussian5_1(gray);
          image_interpolate<Interpolate::Bilinear, float> imgInter =
            image_interpolate<Interpolate::Bilinear, float>(gray);

          int guessCount = 0;
          int totalPass = 0, totalFail = 0;
          SE3 guessPose = pose;
          bool correctPose = false;

          do
            {
              measurements.clear();
              failedPoints.clear();

              int rowPass[gridy+1];
              for(int i = 0; i < gridy+1; i++)
                rowPass[i] = 0;
              int rowFail[gridy+1];
              for(int i = 0; i < gridy+1; i++)
                rowFail[i] = 0;
              int colPass[gridx+1];
              for(int i = 0; i < gridx+1; i++)
                colPass[i] = 0;
              int colFail[gridx+1];
              for(int i = 0; i < gridx+1; i++)
                colFail[i] = 0;

              totalPass = 0;
              totalFail = 0;

              for (size_t i=0; i<grid.size(); i++)
                {
                  int yNo = i/(gridx+1);
                  int xNo = i%(gridx+1);
                  bool blWhite = (xNo+yNo) % 2;
                  if (bottomLeftWhite)
                    blWhite = !blWhite;

                  Vector<2> inPoint = (make_Vector, xNo*cellSize - gridx*cellSize/2, yNo*cellSize - gridy*cellSize/2);
                  Vector<4> likelySquare = (make_Vector, inPoint[0] - cellSize/2, inPoint[1] + cellSize/2,
                                            inPoint[0] + cellSize/2, inPoint[1] - cellSize/2); //l,t,r,b

                  float minMarginDist = minMarginSquare(inPoint, imgInter, guessPose, cameraModel, factor, cellSize);
                  if(minMarginDist > 0)
                    {
                      bool boundsCheck = (minMarginDist < 40) ? true : false;

                      bool okEst = findInitialIntersectionEstimate(imgInter, inPoint, guessPose, cameraModel,
                                   factor, boundsCheck, likelySquare, cellSize);

                      bool pass = false;
                      bool fail = true;

                      if(okEst)
                        {
                          //Find the accurate intersection
                          bool converged = optimiseIntersection(imgInter, inPoint, guessPose, cameraModel, factor,
                                                                boundsCheck, likelySquare, cellSize, blWhite);

                          //Check the found position - inside the image bounds?
                          if(minMarginSquare(inPoint, imgInter, guessPose, cameraModel, factor, cellSize) > 0)
                            {
                              //In expected square?
                              if(inSquareX(inPoint, likelySquare) && inSquareY(inPoint, likelySquare))
                                {
                                  //Optimisation suitably converged?
                                  if(converged)
                                    {
                                      //Sensible difference between the black and white squares?
                                      if(sanityCheck(inPoint, imgInter, guessPose, cameraModel, factor, blWhite, cellSize))
                                        {
                                          pass = true;
                                          fail = false;
                                          rowPass[yNo]++;
                                          colPass[xNo]++;
                                          totalPass++;

                                          //add to the list of points to be used to optimise the pose
                                          measurements.push_back(make_pair(i, imagePoint(inPoint, guessPose, cameraModel, factor)*factor));
                                        }
                                    }
                                }
                            }
                          else
                            fail = false; //Points outside the image haven't really failed
                        }

                      if(fail)
                        {
                          rowFail[yNo]++;
                          colFail[xNo]++;
                          totalFail++;

                          failedPoints.push_back(imagePoint(inPoint, pose, cameraModel, factor));
                        }
                    }
                }

              //If previous frame tracked OK, decision on next one is based on no. of failures
              //Otherwise base on no. of passes - this means if tracking is going OK it will track
              //even when not all points are visible. However if tracking is lost, it will not
              //lock on again until it passes suitably on all rows
              if(prevFrameTracked)
                {
                  int worstRow = rowFail[0];
                  for(int i =0; i<gridy+1; i++)
                    if(rowFail[i] > worstRow)
                      worstRow = rowFail[i];

                  int worstCol = colFail[0];
                  for(int i =0; i<gridx+1; i++)
                    if(colFail[i] > worstCol)
                      worstCol = colFail[i];

                  //allow half of the worst column or row to fail, as long as overall 5x more points pass than fail
                  if(worstCol <= gridx/2 && worstRow <= gridy/2 && static_cast<float>(totalFail)/totalPass < 0.2)
                    correctPose = true;
                }
              else
                {
                  int worstRow = rowPass[0]
                                 ;
                  for(int i =0; i<gridy+1; i++)
                    if(rowPass[i] < worstRow)
                      worstRow = rowPass[i];

                  int worstCol = colPass[0];
                  for(int i =0; i<gridx+1; i++)
                    if(colPass[i] < worstCol)
                      worstCol = colPass[i];

                  //If tracking failed, apply more restrictive matching constraints:
                  //Need at least half the worst/row column to be OK, and 10x more points to pass than fail
                  if(worstCol >= (gridx + 1)/2 && worstRow >= (gridy + 1)/2 &&  static_cast<float>(totalFail)/totalPass < 0.1)
                    correctPose = true;
                }

              if(correctPose)
                {
                  //This pose worked - let's use it
                  pose = guessPose;
                }
              else
                {
                  //Didn't track correctly - let's guess! - just change the pose a bit
                  guessCount++;
                  guessPose = CVD::SE3::exp((make_Vector, rand_g()/300, rand_g()/300, rand_g()/40, rand_g()/10, rand_g()/10, rand_g()/10));
                  //Apply the change in grid coordinates
                  //Grid coordinates actually centred on (0,0,1) in 3D, so need to shift before and after
                  guessPose = SE3::exp((make_Vector, 0, 0, 1, 0, 0, 0)) * guessPose * SE3::exp((make_Vector, 0, 0, -1, 0, 0, 0));
                  guessPose = pose * guessPose;
                }

            }
          while((!correctPose) && (!videoBuffer->frame_pending()));

          if(correctPose)
            {
              if(stage == 2)
                {
                  stage = 1;
                  if(doParams)
                    titlePrefix = "Calibrate: Tracking Pose and Internal Params";
                  else
                    titlePrefix = "Calibrate: Tracking Pose";
                }

              Matrix<6> Cinv;
              pose = findSE3(pose, grid3d, measurements, cameraModel, Cinv, factor*factor);
              if (doParams)
                findParams(pose, grid3d, measurements, cameraModel, factor*factor);

              double now = timer.get_time();
              if (now - lastMeasure > interval)
                {
                  lastMeasure = now;
                  MeasurementSet ms;
                  for (size_t i=0; i<measurements.size(); i++)
                    {
                      ms.im.push_back(measurements[i].second);
                      ms.world.push_back(grid3d[measurements[i].first]);
                    }
                  measurementSets.push_back(ms);
                  poses.push_back(pose);
                  char buf[50];
                  sprintf(buf, " (%u views)", (unsigned int)measurementSets.size());
                  disp.set_title(titlePrefix+buf);
                }
              prevFrameTracked = true;
            }
          else
            prevFrameTracked = false;

          //Draw the grid of estimated pose, and the points used to come up with it
          vector<Vector<2> > img_grid(grid.size());
          for (size_t i=0; i<grid.size(); i++)
            {
              Vector<2> plane = project(pose*grid3d[i]);
              img_grid[i] = cameraModel.project(plane) / factor;
            }
          if(correctPose)
            {
              glColor3f(1,1,1);
              drawGrid (img_grid, gridx, gridy);
              //plot the points used to estimate this pose
              glColor3f(0,1,0);
              for (size_t i=0; i<measurements.size(); i++)
                drawCross(measurements[i].second/factor, 8);
              glColor3f(1,0,0);
              for (size_t i=0; i<failedPoints.size(); i++)
                drawCross(failedPoints[i], 8);
            }
          else
            {
              glColor3f(1,0.2,0.2);
              drawGrid (img_grid, gridx, gridy);
            }

          //Coloured box to signify stage - init, record or calc
          switch(stage)
            {
            case 0:
              glColor4f(0,1,0,0.5);
              break;
            case 1:
              glColor4f(1,0,0,0.5);
              break;
            case 2:
              glColor4f(1,1,0,0.5);
              break;
            }
          glBegin(GL_QUADS);
          glVertex2i(15, 450);
          glVertex2i(30, 450);
          glVertex2i(30, 465);
          glVertex2i(15, 465);
          glEnd();
          disp.swap_buffers();
        }
    }
  /*if (!input.length() && output.length()) {
      ofstream fout(output.c_str());
      fout.precision(19);
      saveData(fout, imageSize, factor, cameraModel.get_parameters(), measurementSets, poses);
  } else if (input.length()) {
      ifstream fin(input.c_str());
      loadData(fin, imageSize, factor, cameraModel.get_parameters(), measurementSets, poses);
  }*/

  size_t numMeasurements = 0;
  for (size_t i=0; i<measurementSets.size(); i++)
    numMeasurements += measurementSets[i].im.size();

  cout.precision(10);
  size_t numPoints = 0;
  for (size_t i=0; i<measurementSets.size(); i++)
    {
      numPoints += measurementSets[i].im.size();
    }
  cout << measurementSets.size() << " sets of measurements, " << numPoints << " total points" << endl;

  double minError = getReprojectionError(measurementSets, poses, cameraModel);
  cout << sqrt(minError/numPoints) / factor << " initial reprojection error" << endl;

  double lambda = 1;
  while (lambda < 1e8)
    {
      Vector<6> params = cameraModel.get_parameters();
      vector<SE3> oldposes = poses;
      improveLM(measurementSets, poses, cameraModel, lambda);
      double error = getReprojectionError(measurementSets, poses, cameraModel);
      if (minError - error > 1e-19)
        {
          minError = error;
          lambda *= 0.5;
          cout << sqrt(minError/numPoints) / factor << endl;
        }
      else
        {
          poses = oldposes;
          cameraModel.get_parameters() = params;
          lambda *= 3;
        }
      Vector<6> v = cameraModel.get_parameters();
      v.slice<0,4>() /= factor;
      cout << v << endl;
    }

  vector<pair<Vector<2>, Vector<2> > > errors;
  pair<double,double> rperr = getReprojectionError(measurementSets, poses, cameraModel, errors);

  if(generateErrorImage){
    cout << "Generating errors.bmp..." << endl;
    Image<float> errorImage(imageSize);
    zeroPixels(errorImage.data(), errorImage.totalsize());
    for (size_t i=0; i<errors.size(); i++)
        {
        ImageRef p = ir_rounded(errors[i].first/factor);
        double mag = sqrt(errors[i].second * errors[i].second / rperr.second);
        errorImage[p] = mag;
        }
    img_save(errorImage, "errors.bmp");
  }

  cout << "Estimating uncertainty..." << endl;
  Matrix<6> Cov;
  getUncertainty(measurementSets, poses, cameraModel, Cov);
  Cov.slice<4,4,2,2>() *= factor*factor;
  Cov.slice<0,4,4,2>() *= factor;
  Cov.slice<4,0,2,4>() *= factor;

  Vector<6> uncertainty;
  for (size_t i=0; i<6; i++)
    uncertainty[i]= 3*sqrt(Cov[i][i]);

  cameraModel.get_parameters().slice<0,4>() /= factor;

  cout.precision(14);
  cout << sqrt(rperr.first/numPoints) / factor << " average reprojection error" << endl;
  cout << "Three sigma uncertainties: " << endl;
  cout << uncertainty << endl << endl;
  cout << "Parameters:" << endl;
  cout << cameraModel.get_parameters() << endl << endl;
  cout << "Covariance:" << endl;
  cout << endl << Cov << endl << endl;

  cout << "Camera Model:" << endl;
  cameraModel.save(cout);
  cout << endl << endl;

  if (output.length())
    {
      ofstream fout(output.c_str());
      fout.precision(19);
      cameraModel.save (fout);
    }

  return 0;
}
