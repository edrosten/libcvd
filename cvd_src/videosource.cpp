#include <cvd/videosource.h>
#include <cvd/byte.h>
#include <cvd/colourspaces.h>

namespace CVD {
	
    void tolower(std::string& s)
    {
	for (size_t i=0; i<s.length(); ++i)
	    s[i] = ::tolower(s[i]);
    }

	void tolower(char* s)
    {
	for (size_t i=0; s[i]; ++i)
	    s[i] = ::tolower(s[i]);
    }

	using std::tolower;
	
	//If the flag is specified at all, but no parameter is
	//given, this implies true.
	bool parseBoolFlag(std::string s)
	{
		tolower(s);
		if(s == "" || s == "yes" || s == "true" || s=="1")
			return 1;
		else if(s == "no" || s == "false" || s=="0")
			return 0;
		else
			throw ParseException("invalid interlaced/fields setting '"+s+"' (must be true/false or yes/no or 1/0)");
	}
	
    std::string escape(char c)
    {
	static std::string escaped[256];
	static bool init = false;
	if (!init) {
	    for (int i=0; i<256; ++i)
		escaped[i] = std::string(1,static_cast<char>(i));
	    escaped[(unsigned char)'\a'] = "\\a";
	    escaped[(unsigned char)'\b'] = "\\b";
	    escaped[(unsigned char)'\f'] = "\\f";
	    escaped[(unsigned char)'\n'] = "\\n";
	    escaped[(unsigned char)'\r'] = "\\r";
	    escaped[(unsigned char)'\t'] = "\\t";
	    escaped[(unsigned char)'\v'] = "\\v";
	    escaped[(unsigned char)'\\'] = "\\\\";
	    escaped[(unsigned char)'\''] = "\\'";
	    escaped[(unsigned char)'\"'] = "\\\"";
	    init = true;
	}
	return escaped[(unsigned char)c];
    }

    std::string unescape(const std::string& s)
    {
	static const char mapping[][2] = {{'a','\a'},
					  {'b','\b'},
					  {'f','\f'},
					  {'n','\n'},
					  {'r','\r'},
					  {'t','\t'},
					  {'v','\v'},
					  {'\\','\\'},
					  {'\'','\''},
					  {'"','"'},
					  {0,0}};
	std::string ret;
	ret.reserve(s.length());
	for (size_t i=0; i<s.length(); ++i) {
	    if (s[i] == '\\') {
		if (i + 1 == s.length())
		    throw ParseException("illegal '\\' terminating literal");
		char escape = s[i+1];
		if (isdigit(escape)) {
		    if (i + 3 >= s.length() || (s[i+2] < '0' || s[i+2] > '7') || (s[i+3] < '0' || s[i+3] > '7'))
			throw ParseException("partial octal character code; need three digits");
		    int code = (escape-'0')*64 + (s[i+2] - '0')*8 + s[i+3] - '0';
		    if (code > 255)
			throw ParseException("invalid octal character code; must be in [000,377]");
		    ret += static_cast<char>(code);
		    i += 3;
		} else if (escape == 'h') {
		    if (i+3 >= s.length() || !isxdigit(s[i+2]) || !isxdigit(s[i+3]))
			throw ParseException("partial hex character code; need two hex digits");
		    int code = (isdigit(s[i+2]) ? (s[i+2]-'0') : (tolower(s[i+2])-'a' + 10))*16 +
			(isdigit(s[i+3]) ? (s[i+3]-'0') : (tolower(s[i+3])-'a' + 10));
		    ret += static_cast<char>(code);
		    i += 3;
		} else {
		    int j=0;
		    while (mapping[j][0] && mapping[j][0] != s[i+1])
			++j;
		    if (!mapping[j][0])
			throw ParseException("unknown escape sequence");
		    ret += mapping[j][1];
		    i += 1;
		}
	    } else
		ret += s[i];
	}
	return ret;
    }

    std::ostream& operator<<(std::ostream& out, const VideoSource& vs)
    {
	out << vs.protocol << ":";
	if (!vs.options.empty()) {
	    out << "[ ";
	    for (VideoSource::option_list::const_iterator it = vs.options.begin(); it != vs.options.end(); ++it) {
		out << it->first << "=" << it->second << ", ";
	    }
	    out << "]";
	}
	out << "//" << vs.identifier;
	return out;
    }

    void match(std::istream& in, char c) {
	if (in.peek() != c) {
	    std::ostringstream oss;
	    oss << "expected '" << c <<"', got '" << escape(in.peek()) << "'";
	    throw ParseException(oss.str());
	}
	in.ignore();
    }

    void skip_ws(std::istream& in) {
	while (isspace(in.peek())) 
	    in.ignore();
    }

    std::string read_quoted_literal(std::istream& in)
    {
	std::string value;
	match(in, '"');
	while (in.peek() != '"') {
	    char c = in.get();
	    value += c;
	    if (c == '\\')
		value += in.get();
	}
	match(in,'"');
	return value;
    }

    std::string read_word(std::istream& in)
    {
	std::string word;
	while (isalnum(in.peek()) || in.peek()=='_')
	    word += in.get();		
	return word;
    }

    std::string read_path(std::istream& in)
    {
	std::string word;
	while (isgraph(in.peek()))
	    word += in.get();
	return word;
    }



    void parse(std::istream& in, VideoSource& vs)
    {
	std::string prot;
	skip_ws(in);
	while (isalnum(in.peek()))
	    prot += in.get();
	if (prot.length() == 0) 
	    throw ParseException("protocol must not be empty");
	vs.protocol = prot;
	//std::cerr << "prot" << std::endl;
	if (in.peek() != ':')
	    throw ParseException("expected ':' after protocol");
	in.ignore();
	if (in.peek() == '[') {
	    in.ignore();
	    while (true) {
		skip_ws(in);
		std::string name = read_word(in);
		tolower(name);
		//std::cerr << "opt: " << name << std::endl;
		skip_ws(in);	
	    
		if (in.peek() != '=')
		    vs.options.push_back(std::make_pair(name,""));
		else {
		    in.ignore();
		    skip_ws(in);
		    if (in.peek() == '"') {
			vs.options.push_back(std::make_pair(name,unescape(read_quoted_literal(in))));
		    } else {
			vs.options.push_back(std::make_pair(name,read_word(in)));
		    }
		}
		skip_ws(in);
		if (in.peek() != ',')
		    break;
		in.ignore();
	    }
	    skip_ws(in);
	    match(in, ']');
	    //std::cerr << "options" << std::endl;
	}
	match(in, '/');
	match(in, '/');
	//std::cerr << "slashes" << std::endl;
	if (in.peek() == '"') {
	    std::string id = read_quoted_literal(in);
	    vs.identifier = unescape(id);
	    //std::cerr << "quoted id" << std::endl;
	} else {
	    std::string id = read_path(in);
	    vs.identifier = id;
	    //std::cerr << "id" << std::endl;
	}
    
    }

    void get_jpegstream_options(const VideoSource& vs, int& ra_frames)
    {
	ra_frames = 0;

	for (VideoSource::option_list::const_iterator it=vs.options.begin(); it != vs.options.end(); ++it) {	    
	    if (it->first == "read_ahead") {
		ra_frames = 50;
		if (it->second.length())
		    ra_frames = atoi(it->second.c_str());
	    }
	    else
		throw VideoSourceException("invalid option for files protocol: "+it->first +
					   "\n\t valid options: read_ahead");
	}
    }

    void get_files_options(const VideoSource& vs, int& fps, int& ra_frames, VideoBufferFlags::OnEndOfBuffer& eob)
    {
	fps = 30;
	ra_frames = 0;
	eob = VideoBufferFlags::RepeatLastFrame;
	for (VideoSource::option_list::const_iterator it=vs.options.begin(); it != vs.options.end(); ++it) {	    
	    if (it->first == "fps") {
		fps = atoi(it->second.c_str());
	    } else if (it->first == "read_ahead") {
		ra_frames = 50;
		if (it->second.length())
		    ra_frames = atoi(it->second.c_str());
	    } else if (it->first == "on_end") {		
		if (it->second == "loop") 
		    eob = VideoBufferFlags::Loop;
		else if (it->second == "unset_pending")
		    eob = VideoBufferFlags::UnsetPending;
		else if (it->second == "repeat_last")
		    eob = VideoBufferFlags::RepeatLastFrame;
		else
		    throw VideoSourceException("invalid end-of-buffer behaviour: "+it->second+"\n\t valid options are repeat_last, unset_pending, loop");
	    } else
		throw VideoSourceException("invalid option for files protocol: "+it->first +
					   "\n\t valid options: read_ahead, on_end, fps");
	}
    }



#if CVD_HAVE_V4L1BUFFER
    template <> CVD::VideoBuffer<CVD::byte>* makeV4L1Buffer(const std::string& dev, const CVD::ImageRef& size)
    {
	return new CVD::V4L1Buffer<CVD::byte>(dev, size);
    }

    template <> CVD::VideoBuffer<CVD::yuv422>* makeV4L1Buffer(const std::string& dev, const CVD::ImageRef& size)
    {
	return new CVD::V4L1Buffer<CVD::yuv422>(dev, size);
    }
    template <> CVD::VideoBuffer<CVD::Rgb<CVD::byte> >* makeV4L1Buffer(const std::string& dev, const CVD::ImageRef& size)
    {
	return new CVD::V4L1Buffer<CVD::Rgb<CVD::byte> >(dev, size);
    }

    template <> CVD::VideoBuffer<CVD::bayer_grbg>* makeV4L1Buffer(const std::string& dev, const CVD::ImageRef& size)
    {
	return new CVD::V4LBuffer<CVD::bayer_grbg>(dev, size);
    }

    void get_v4l1_options(const VideoSource& vs, ImageRef& size)
    {
	size = ImageRef(0,0);

	for (VideoSource::option_list::const_iterator it=vs.options.begin(); it != vs.options.end(); ++it) {
	    if (it->first == "size") {		
		std::string s = it->second;
		tolower(s);
		if (s == "vga")
		    size = ImageRef(640,480);
		else if (s == "qvga")
		    size = ImageRef(320,240);
		else if (s == "pal") 
		    size = ImageRef(720,576);
		else if (s == "ntsc")
		    size = ImageRef(720,480);
		else {
		    std::istringstream size_in(s);
		    char x;
		    if (!(size_in >> size.x >> x >> size.y))
			throw ParseException("invalid image size specification: '"+it->second+"'\n\t valid specs: vga, qvga, pal, ntsc, <width>x<height>");
		}
	    } else
		throw VideoSourceException("invalid option for 'v4l2' protocol: "+it->first+"\n\t valid options: size, input, interlaced, fields");
	}
    }

#endif


#if CVD_INTERNAL_HAVE_V4LBUFFER
    template <> CVD::VideoBuffer<CVD::byte>* makeV4LBuffer(const std::string& dev, const CVD::ImageRef& size, int input, bool interlaced, bool verbose)
    {
	return new CVD::V4LBuffer<CVD::byte>(dev, size, input, interlaced, 0, verbose);
    }

    #ifdef V4L2_PIX_FMT_SBGGR8
	template <> CVD::VideoBuffer<CVD::bayer_grbg>* makeV4LBuffer(const std::string& dev, const CVD::ImageRef& size, int input, bool interlaced, bool verbose)
	{
	    return new CVD::V4LBuffer<CVD::bayer_grbg>(dev, size, input, interlaced, 0, verbose);
	}
    #endif
    template <> CVD::VideoBuffer<CVD::yuv422>* makeV4LBuffer(const std::string& dev, const CVD::ImageRef& size, int input, bool interlaced, bool verbose)
    {
	return new CVD::V4LBuffer<CVD::yuv422>(dev, size, input, interlaced, 0, verbose);
    }
    template <> CVD::VideoBuffer<CVD::vuy422>* makeV4LBuffer(const std::string& dev, const CVD::ImageRef& size, int input, bool interlaced, bool verbose)
    {
	return new CVD::V4LBuffer<CVD::vuy422>(dev, size, input, interlaced, 0, verbose);
    }
    template <> CVD::VideoBuffer<CVD::Rgb<CVD::byte> >* makeV4LBuffer(const std::string& dev, const CVD::ImageRef& size, int input, bool interlaced, bool verbose)
    {
	return new CVD::V4LBuffer<CVD::Rgb<CVD::byte> >(dev, size, input, interlaced, 0, verbose);
    }

    template <> CVD::VideoBuffer<CVD::Rgb8>* makeV4LBuffer(const std::string& dev, const CVD::ImageRef& size, int input, bool interlaced, bool verbose)
    {
	return new CVD::V4LBuffer<CVD::Rgb8>(dev, size, input, interlaced, 0, verbose);
    }

    void get_v4l2_options(const VideoSource& vs, ImageRef& size, int& input, bool& interlaced, bool& verbose)
    {
	size = ImageRef(640,480);
	input = -1;
	interlaced = false;
	verbose=0;
	for (VideoSource::option_list::const_iterator it=vs.options.begin(); it != vs.options.end(); ++it) {
	    if (it->first == "size") {		
		std::string s = it->second;
		tolower(s);
		if (s == "vga")
		    size = ImageRef(640,480);
		else if (s == "qvga")
		    size = ImageRef(320,240);
		else if (s == "pal") 
		    size = ImageRef(720,576);
		else if (s == "ntsc")
		    size = ImageRef(720,480);
		else {
		    std::istringstream size_in(s);
		    char x;
		    if (!(size_in >> size.x >> x >> size.y))
			throw ParseException("invalid image size specification: '"+it->second+"'\n\t valid specs: vga, qvga, pal, ntsc, <width>x<height>");
		}
	  } else if (it->first == "input") {
		input = atoi(it->second.c_str());
	    } else if (it->first == "verbose") {
			verbose = parseBoolFlag(it->second);
	    } else if (it->first == "interlaced" || it->first == "fields") {
			interlaced = parseBoolFlag(it->second);
	    } else
		throw VideoSourceException("invalid option for 'v4l2' protocol: "+it->first+"\n\t valid options: size, input, interlaced, fields");
	}
    }

#endif

#if CVD_HAVE_FFMPEG
    template <> CVD::VideoBuffer<CVD::byte>* makeVideoFileBuffer(const std::string& file, CVD::VideoBufferFlags::OnEndOfBuffer eob)
    {
	CVD::VideoFileBuffer<CVD::byte>* vb = new CVD::VideoFileBuffer<CVD::byte>(file);
	vb->on_end_of_buffer(eob);
	return vb;
    }

    template <> CVD::VideoBuffer<CVD::Rgb<CVD::byte> >* makeVideoFileBuffer(const std::string& file, CVD::VideoBufferFlags::OnEndOfBuffer eob)
    {
	CVD::VideoFileBuffer<CVD::Rgb<CVD::byte> >* vb = new CVD::VideoFileBuffer<CVD::Rgb<CVD::byte> >(file);
	vb->on_end_of_buffer(eob);
	return vb;
    }

    void get_file_options(const VideoSource& vs, int& ra_frames, VideoBufferFlags::OnEndOfBuffer& eob)
    {
	eob = VideoBufferFlags::RepeatLastFrame;
	for (VideoSource::option_list::const_iterator it=vs.options.begin(); it != vs.options.end(); ++it) {
	    if (it->first == "read_ahead") {
		ra_frames = 50;
		if (it->second.length())
		    ra_frames = atoi(it->second.c_str());
	    } else if (it->first == "on_end") {		
		if (it->second == "loop") 
		    eob = VideoBufferFlags::Loop;
		else if (it->second == "unset_pending")
		    eob = VideoBufferFlags::UnsetPending;
		else if (it->second == "repeat_last")
		    eob = VideoBufferFlags::RepeatLastFrame;
		else
		    throw VideoSourceException("invalid end-of-buffer behaviour: "+it->second+"\n\t valid options are repeat_last, unset_pending, loop");
	    } else
		throw VideoSourceException("invalid option for 'file' protocol: "+it->first+"\n\t valid options: read_ahead, on_end, fps");
	}
    }

#endif

#if CVD_HAVE_DVBUFFER
    template <> CVD::VideoBuffer<CVD::byte>* makeDVBuffer2(int cam, int dmabufs, int bright, int exposure, int fps)
    {
	return new CVD::DVBuffer2<CVD::byte>(cam,dmabufs,bright,exposure,fps);
    }

    template <> CVD::VideoBuffer<CVD::Rgb<CVD::byte> >* makeDVBuffer2(int cam, int dmabufs, int bright, int exposure, int fps)
    {
	return new CVD::DVBuffer2<CVD::Rgb<CVD::byte> >(cam,dmabufs,bright,exposure,fps);
    }

    void get_dc1394_options(const VideoSource& vs, int& dma_bufs, int& bright, int& exposure, int& fps)
    { 
	dma_bufs = 3;
	bright = -1;
	exposure = -1;
	fps = 30;
	for (VideoSource::option_list::const_iterator it=vs.options.begin(); it != vs.options.end(); ++it) {
	    if (it->first == "fps")
		fps = atoi(it->second.c_str());
	    else if (it->first == "dma_bufs" || it->first == "dma_buffers")
		dma_bufs = atoi(it->second.c_str());
	    else if (it->first == "brightness" || it->first == "bright")
		bright = atoi(it->second.c_str());
	    else if (it->first == "exp" || it->first == "exposure")
		exposure = atoi(it->second.c_str());
	    else
		throw VideoSourceException("invalid option for dc1394 protocol: "+it->first+"\n\t valid options: dma_bufs, brightness, exposure, fps");
	}
   }
#endif

#if CVD_HAVE_QTBUFFER
    template <> VideoBuffer<vuy422> * makeQTBuffer( const ImageRef & size, int input, bool showsettings)
    {
        return new CVD::QTBuffer<vuy422>(size, input, showsettings);
    }

    template <> VideoBuffer<yuv422> * makeQTBuffer( const ImageRef & size, int input, bool showsettings)
    {
        return new CVD::QTBuffer<yuv422>(size, input, showsettings);
    }
    
    void get_qt_options(const VideoSource & vs, ImageRef & size, bool & showsettings){
	size = ImageRef(640, 480);
	showsettings = false;
	for (VideoSource::option_list::const_iterator it=vs.options.begin(); it != vs.options.end(); ++it) {
		if (it->first == "size") {
		std::string s = it->second;
		tolower(s);
		if (s == "vga")
		    size = ImageRef(640,480);
		else if (s == "qvga")
		    size = ImageRef(320,240);
		else {
		    std::istringstream size_in(s);
		    char x;
		    if (!(size_in >> size.x >> x >> size.y))
			throw ParseException("invalid image size specification: '"+it->second+"'\n\t valid specs: vga, qvga, <width>x<height>");
		}
		}
		else if(it->first == "showsettings") {
		    showsettings = atoi(it->second.c_str());
	    } else
		throw VideoSourceException("invalid option for 'qt' protocol: "+it->first+"\n\t valid options: size, showsettings");
	}
    }
#endif

}
