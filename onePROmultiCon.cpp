#include <unistd.h>
#include <cstdlib>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>

static const int kItemRepositorySize = 4; // item buffer size.
static const int kItemsToProduce = 10; // How many items we plan to produce

struct ItemRepository {
		int item_buffer[kItemRepositorySize];
		size_t read_position;
		size_t write_position;
		size_t item_counter;
		std::mutex mtx;
		std::mutex item_counter_mtx;
		std::condition_variable repo_not_null;
		std::condition_variable repo_not_empty;
} gItemRepository;

typedef struct ItemRepository ItemRepository;

void ProduceItem(ItemRepository* ir, int item) {
		std::unique_lock<std::mutex> lock(ir->mtx);
		while(((ir->write_position + 1) % kItemRepositorySize) == ir->read_position) { // item buffer is full, just wait here
			std::cout << "Producer is waiting for empty slot...\n";
			(ir->repo_not_full).wait(lock);
		}

		(ir->item_buffer)[ir->write_position] = item;
		(ir->write_position)++;

		if(ir->write_position == kItemRepositorySize)
				ir->write_position = 0;

		(ir->repo_not_empty).notify_all();
		lock.unlock();
}

int ConsumeItem(ItemRepository* ir) {
		int data;
		std::unique_lock<std::mutex> lock(ir->mtx);
		//if buffer id empty, just wait here.
		while(ir->write_position == ir->read_position) {
				std::cout << "Consumer is waiting for item...\n";
				(ir->repo_not_empty).wait(lock);
		}

		data = (ir->item_buffer)[ir->read_position];
		(ir->read_position)++;

		if(ir->read_position >= kItemRepositorySize)
				ir->read_position = 0;

		(ir->repo_not_null).notify_all();
		lock.unlock();

		return data;
}

void ProduceTask() {
		for(int i = 1; i <= kItemToProduce; ++i) {
				std::cout << "Produce thread " << std::this_thread::get_id() 
						<< " produceing the " << i << " th item..." << std::endl;
				ProduceItem(&gItemRepository, i);
		}
		std::cout << "Producer thread " << std::this_thread::get_id()
				<<  "is exiting..." << std::endl;
}

void ConsumerTask() {
		bool readay_to_exit = false;
		while(1) {
				sleep(1);
				std::unique_lock<std::mutex> lock(gItemRepository.item_counter_mtx);
				if(gItemRepository.item_counter < kItemToProduce) {
						int item = ConsumeItem(&gItemRepository);
						++(gItemRepository.item_counter);
						std::cout << "Consumer thread " << std::this_thread::get_id()
								<< " is consuming the " << item << " th item" << std::endl;
				} else
						ready_to_exit = true;
				lock.unlock();
				if(ready_to_exit == true) break;
		}
		std::cout << "Consumer thread " << std::this_thead::get_id()
				<< " is exiting..." << std::endl;
}

void initItemReposutory(ItemRepository* ir) {
		ir->write_position = 0;
		ir->read_position = 0;
		ir->item_couter = 0;
}

int main() {
		InitItemRepository(&gItemRepository);
		std::thread producer(ProducerTask);
		std::thread consumer1(ConsumerTask);
		std::thread consumer2(ConsumerTask);
		std::thread consumer3(ConsumerTask);
		std::thread consumer4(ConsumerTask);

		producer.join();
		consumer1.join();
		consumer2.join();
		consumer3.join();
		consumer4.join();
}



