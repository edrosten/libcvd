#include "cvd/runnable_batch.h"
#include "cvd/thread.h"
#include "cvd/message_queue.h"

namespace CVD
{

class RunnableBatch::RunMessageInThread: public Thread
{
	public:
		RunMessageInThread(MessageQueue<CVD::STD::shared_ptr<Runnable> >* queue);
		virtual void run();

	private:
		MessageQueue<CVD::STD::shared_ptr<Runnable> >* q;
};


RunnableBatch::RunMessageInThread::RunMessageInThread(MessageQueue<CVD::STD::shared_ptr<Runnable> >* queue)
:q(queue)
{}


void RunnableBatch::RunMessageInThread::run()
{
	CVD::STD::shared_ptr<Runnable> r;

	while( (r = q->read()).get() != NULL )
		r->run();
}

RunnableBatch::RunnableBatch(unsigned int p)
    :joined(0),parallelism(p),queue(new MessageQueue<CVD::STD::shared_ptr<Runnable> >())
{
	//Create and start threads
	for(unsigned int i=0; i < parallelism; i++)
	{
		threads.push_back(CVD::STD::shared_ptr<RunMessageInThread>(new RunMessageInThread(queue.get())));
		threads.back()->start();
	}
}

void RunnableBatch::join()
{
	if(!joined)
	{
		//Send just enough termination messages
		for(unsigned int i=0; i < threads.size(); i++)
			queue->write(CVD::STD::shared_ptr<Runnable>() );
		

		//Wait for all threads to complete. This will occur when
		//all pending tasks have been run
		for(unsigned int i=0; i < threads.size(); i++)
			threads[i]->join();

		joined = 1;
	}
}

void RunnableBatch::schedule(CVD::STD::shared_ptr<Runnable> r)
{
	if(parallelism > 0)	
		queue->write(r);
	else
		r->run();
}

RunnableBatch::~RunnableBatch()
{
	join();
}
}
