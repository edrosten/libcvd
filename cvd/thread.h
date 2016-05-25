#ifndef CVD_INCLUDE_THREAD_H
#define CVD_INCLUDE_THREAD_H

//POSIX threads
#include <thread>
#include <atomic>

namespace CVD {
	//! This is an abstract base class for anything with a run() method.
	class [[deprecated("Use std::function instead")]] Runnable 
	{
		public:
			//! Perform the function of this object.
			virtual void run()=0;
			virtual ~Runnable(){};
	};

	/** Thread class. Used for portability pre C++11.
		Deprecated. Use std::thread.
	 */

	
	class [[deprecated("Use std::thread instead")]] Thread : public Runnable
	{
		public:
			Thread(const Thread&) = delete;

			//! Construct a thread.  If runnable != 0, use that runnable, else use our own "run" method.
			Thread()
			{
			};

			//! This does not destroy the object until the thread has been terminated.
			virtual ~Thread()
			{
				stop();
				if(my_thread.joinable())
					join();
			}

			//! Start execution of "run" method in separate thread.
			void start(Runnable* runnable=0)
			{
				myRunnable = runnable ? runnable : this;
				myStopFlag = false;

				ourCount++;
				
				//Detatch the thread if it's running.
				if(my_thread.joinable())
					my_thread.detach();

				//Create a new thread.
				my_thread = std::thread([&]()
				{
					current = this;
					myRunningFlag = true;
					myRunnable->run();
					myRunningFlag = false;
				});
			}


			//! Tell the thread to stop.  
			/** This doesn't make the thread actually stop, it just causes shouldStop() to return true. */
			void stop()
			{
				myStopFlag = true;
			}

			//! Returns true if the stop() method been called, false otherwise.
			bool shouldStop() const
			{
				return myStopFlag;
			}

			//! Returns true if the thread is still running.
			bool isRunning() const;

			//! This blocks until the thread has actually terminated.  
			/** If the thread is infinite looping, this will block forever! */
			void join()
			{
				my_thread.join();
			}

			//! Get the ID of this thread.
			std::thread::id getID()
			{
				return my_thread.get_id();
			}

			//! Override this method to do whatever it is the thread should do.
			virtual void run(){};

			//Static methods:

			//! Returns how many threads are actually running, not including the main thread.
			static unsigned int count()
			{
				return ourCount;
			};

			//! Returns a pointer to the currently running thread.
			static Thread* getCurrent()
			{
				return current;
			}

			//! Tell the current thread to sleep for milli milliseconds
			static void sleep(unsigned int milli)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(milli));
			}

			//! Tell the current thread to yield the processor.
			static void yield()
			{
				std::this_thread::yield();
			}

		private:
			static std::atomic<unsigned int> ourCount;
			static thread_local Thread* current;

			Runnable* myRunnable = nullptr;
			std::thread my_thread;

			std::atomic<bool> myRunningFlag{ false };
			std::atomic<bool> myStopFlag{ false };
	};

}
#endif
