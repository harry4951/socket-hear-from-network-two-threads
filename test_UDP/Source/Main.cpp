/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include <fstream>
using namespace std;

////==============================================================================
//int main (int argc, char* argv[])
//{
//	float* Buff;
//	int si_port = 8888;
//	int MaxreadByte = 32;
//    // ..your code goes here!
//	DatagramSocket sock;
//	String si_other;
//	
//	
//	if (!sock.bindToPort(si_port))
//		{
//			printf("Error: could not connect to UDP port 8888.\n");
//		}
//		_sleep(1000);
//		
//		ofstream myfile;
//		myfile.open("example.txt");
//		//while (getchar()!='c')
//		while (1)
//		{
//			printf("Waiting for receiving data......\n");
//			fflush(stdout);
//			
//			
//			ofstream outfile("test.txt", fstream::app);
//
//			if (sock.waitUntilReady(true, -1) == 1)
//			{
//				//myfile.open("example.txt");
//				
//				int tmp = sock.read(Buff, MaxreadByte, false, si_other, si_port);
//				printf("Received data %d bytes\n", tmp);
//				
//				
//					
//				for (int i = 0; i < tmp-3; i++)
//				{
//					cout << Buff[i]<<endl ;
//					outfile << Buff[i] << endl;
//					//outfile.close();
//					//outfile.flush();
//				}
//				
//
//				//cout << si_other << endl;
//				//myfile << Buff <<".\n";
//				//myfile.close();
//				//char *Buf = {"HELLO H\n'\0"};
//				
//				/*if(sock.write(si_other, si_port, Buff, MaxreadByte)==-1)
//				{
//					printf("Error writing...\n");
//				}*/
//			}
//			
//			
//			
//			
//		}
//
//		myfile.close();
//	
//
//
//    return 0;
//}


#include <condition_variable>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>

template <typename T>
class Queue
{
public:

	T pop()
	{
		unique_lock<mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		auto item = queue_.front();
		queue_.pop();
		return item;
	}

	void pop(T& item)
	{
		unique_lock<mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		item = queue_.front();
		queue_.pop();
	}

	void push(const T& item)
	{
		unique_lock<mutex> mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
	}

private:
	queue<T> queue_;
	mutex mutex_;
	condition_variable cond_;
};

class Producer
{
public:
	Producer(Queue<float>& buffer)
		: queue_(buffer)
	{}

	void start_sock()
	{
		if (!sock.bindToPort(si_port))
		{
			printf("Error: could not connect to UDP port 8888.\n");
		}
		_sleep(1000);
	}
	void run() {
		while (true) {


			printf("Waiting for receiving data......\n");
			fflush(stdout);

			if (sock.waitUntilReady(true, -1) == 1)
			{
				//myfile.open("example.txt");

				int tmp = sock.read(Buff, MaxreadByte, false, si_other, si_port);
				printf("Received data %d bytes\n", tmp);



				for (int i = 0; i < tmp - 3; i++)
				{
					cout << "Produced: "<<Buff[i] << endl;
					queue_.push(Buff[i]);
				}
			}

			
			//this_thread::sleep_for(chrono::seconds(1));

		}
	}
private:
	Queue<float>&  queue_;
	DatagramSocket sock;

	float Buff[32];
	int si_port = 8888;
	int MaxreadByte = 32;
	String si_other;
};

class Consumer
{
public:

	Consumer(Queue<float>& buffer)
		: queue_(buffer)
	{}

	void run() {
		while (true) {
			float num = queue_.pop();
			cout << "Consumed: " << num << endl;
			this_thread::sleep_for(chrono::seconds(1));
		}
	}
private:
	Queue<float>&  queue_;
};

//==============================================================================
int main(int argc, char* argv[])
{


	Queue<float> b;
	Producer p(b);
	Consumer c(b);

	p.start_sock();

	thread producer_thread(&Producer::run, &p);
	thread consumer_thread(&Consumer::run, &c);

	producer_thread.join();
	consumer_thread.join();

	return 0;
}
