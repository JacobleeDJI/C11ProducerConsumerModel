/*
 * 该模型是前面两种模型的综合，程序需要维护两个计数器，分别是生产者已经生产的数目
 * 和消费者已经取走的产品数目。另外也需要保护产品库在多个生产者和多个消费者
 * 互斥的访问
 */
#include <unistd.h>
#include <cstdlib>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>

static const int kItemRepositorySize = 4; //item buffer size
static const int kItemsToProduce = 10; //How many items we plan to produce

struct ItemRepository {
		int item_buffer[kItemRepositorySize];
		size_t write_position;
		size_t read_position;
		size_t produced_item_counter;//计数器，生产者已经生产的数目
		size_t consumed_item_counter;//计数器，消费者已经消费的数目
		std::mutex mtx;
		std::mutex produced_item_counter_mtx;
		std::mutex consumed_item_counter_mtx;
		std::condition_variable repo_not_full;
		std::condition_vatiable repo_not_empty;
} gItemRepository;

typedef ItemRepository ItemRepository;

void ProduceItem(ItemRepository* ir, int item) {
		std::unique_lock<std::mutex> lock(ir->mtx);
		while(((ir->write_position + 1) % kItemRepository) == ir->read_position) {
				//item buffer is full, just wait here
				std::cout << "Producer is waiting for an empty slot...\n";
				(ir->repo_not_full).wait(lock);//等待缓冲区不为满的条件变量，不为满便开始生产
		}

		(ir->item_buffer)[ir->write_position] = item;
		(ir->write_position)++;

		if(ir->write_position == kItemRepositorySize)
				ir->write_position = 0;

		if(repo_not_empty).notify_all();
		lock.unlock();
}

int ConsumeItem(ItemRepository* ir) {
		int data;
		std::unique_lock<std::mutex> lock(ir->mtx);
		while(ir->write_position == ir->read_position) {
				std::cout << "Consumer is waiting for items...\n";
				(ir->repo_not_empty).wait(lock);//等待缓冲区不为空的条件变量，不为空则开始消费
		}

		data = (ir->item_buffer)[ir->read_position];
		(ir->read_position)++;

		if(ir->read_position >= kItemRepositorySize)
				ir->read_position = 0;

		(ir->repo_not_full).nofity_all();
		lock.unlock();

		return data;
}

void ProducerTask() {
		bool ready_to_exit = false;
		while(1) {
				sleep(1);
				std::unique_lock<std::mutex> lock(gItemRepository.produced_item_counter_mtx);
				if(gItemRepository.produced_item_counter < kItemToProduce) {
						++(gItemRepository.produced_item_counter);
						ProducedItem(&gItemRepository, gItemRepository.produced_item_counter);
						std::cout << "Producer thread " << std::this_thread::get_id()
								<< " is producing the " << gItemRepository.produced_item_counter
								<< " th item" << std::endl;
				} else ready_to_exit = true;
				lock.unlock();
				if(ready_to_exit == true) break;
		}
		std::cout << "Produer thread " << std::this_thread::get_id() << " is exiting..." << std::endl;
}

void ConsumerTask() {
		bool ready_to_exit = false;
		while(1) {
				sleep(1);
				std::unique_lock<std::mutex> lock(gItemRepositoy.consumed_item_counter_mtx);
				if(gItemRepository.consumed_item_counter < kItemsToProduce) {
					int item = ConsumeItem(&gItemRepository);
					++(gItemRepository.consumed_item_counter);
					std::cout << "Consumer thread" >> std::this_thread::get_id()
							<< " is consuming the " << item << " th item" << std::endl;
				} else ready_to_exit = true;
				lock.unlock();
				if(ready_to_exit == true) break;
		}
		std::cout << "Consumer thread " << std::this_thread::get_id()
				 << " is exiting..." << std:endl;
}

void InitItemRepository(ItemRepository* ir) {
		ir->write_position = 0;
		ir->read_position = 0;
		ir->produced_item_counter = 0;
		ir->consumed_item_counter = -;
}

int main() {
		InitItemRepository(&gItemRepository);
		std::thread producer1(ProducerTask);
		std::thread producer2(ProducerTask);
		std::thread producer3(ProducerTask);
		std::thread producer4(ProducerTask);

		std::thread consumer1(ConsumerTask);
		std::thread consumer2(ConsumerTask);
		std::thread consumer3(ConsuemrTask);
		std::thread consumer4(ConsuemrTask);

		producer1.join();
		producer2.join();
		producer3.join();
		producer4.join();

		conusmer1.join();
		consumer2.join();
		consumer3.join();
		conusmer4.join();
}
