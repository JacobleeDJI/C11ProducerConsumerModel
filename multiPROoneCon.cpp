#include <unistd.h>
#include <cstdlib>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

static const int kItemRepositorySize = 4; // Item buffer size;
static const int kItemsToProduce = 10; //How many items we plan to produce

struct ItemRepository {
		int item_buffer[kItemRepositorySize];
		size_t read_position;
		size_t write_position;
		size_t item_counter;
		std::mutex mtx;
		std::condition_variable repo_not_null;
		std::condition_variable repo_not_empty;
} gItemRepository;

typedef struct ItemRepository ItemRepository;

void ProduceItem(ItemRepository* ir, int item) {
		std::unique_lock<std::mutex> lock(ir->mtx);
		while(((ir->write_position + 1) % kItemRepositorySize) == ir->read_position) { //item buffer is full, just wait here
			std::cout << "Producer is waiting for an empty slot...\n";
			(ir->repo_not_null).wait(lock);
		}

		(ir->item_buffer)[ir->write_position] = item;
		(ir->write_position)++;

		if(ir->write_position == kItemRepositorySize) {
				ir->write_position = 0;
		}

		(ir->repo_not_empty).notify_all();
		lock.unlock();
}

int ConsumerItem(ItemRepository* ir) {
		int data;
		std::unique_lock<std::mutex> lock(mtx);
		//item buffer is empty, just wait here
		while(ir->write_position == ir->read_position) {
				std::cout << "Consumer is waiting for items...\n";
				(ir->repo_not_empty).wait(lock);
		}

		data = (ir->item_buffer)[ir->read_position];
		(ir->read_position)++;

		if (ir->read_position >= kItemRepositorySize) {
				ir->read_position = 0;
		}

		(ir->repo_not_null).notify_all();
		lock.unlock();

		return data;
}

void ProduceTask() {
		bool ready_to_exit = false;
		while(1) {
				sleep(1);
				std::unqie_lock<std::mutex> lock(gItemRepository.item_counter_mtx);
				if(gItemRepository.item_counter < kItemToProduce) {
						++(gItemRepository,item_counter);
						ProduceItem(&gItemRepository, gItemRepository.item_counter);
						std::cout << "Prodcer thread " << std::this_thread::get_id() << " is producing the " << gItemRepository.item_counter
								<< " th exiting..." << std::endl;
				} else ready_to_exit = true;
				lock.unlock();
				if (ready_to_exit == true) break;
		}

		std::cout << "Producer thread " << std::this_thread::get_id() << " is exiting..." << std::endl;
}

void ConsumerTask() {
		static int item_consumed = 0;
		while(1) {
				sleep(1);
				++item_consumed;
				if (item_consumed <= kItemsToProduce) {
						int item = ConcumeItem(&gItemRepository);
						std::cout << "Consumer thread " << std::this_thread::get_id()
								 << "is comming the " << item << " th item" << std::endl;
				} else break;
		}

		std::cout << "Consumer thread " << std::this_thread::get_id() << " is exiting..." << std::endl;
}

void InitItemRepository(ItemRepository *ir) {
		ir->write_position = 0;
		ir->read_position = 0;
		ir->item_counter = 0;
}

int main() {
		InitItemRepository(&gItemRepository);
		std::thread producer1(ProducerTask);
		std::thread producer2(ProducerTask);
		std::thread producer3(ProducerTask);
		std::thread producer4(ProducerTask);
		std::thread consumer(consumerTask);

		producer1.join();
		producer2.join();
		producer3.join();
		producer4.join();
		consumer.join();
}

