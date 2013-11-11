#include <iostream>
#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <stdlib.h>

enum buy_or_sell{BUY, SELL};

using std::string;

class market_maker{
	public:
	market_maker() : current_id(0), current_timestamp(0) {};
	
	// Each one of these structures will hold one particular type of equity, in a
	// vector of "stocks"
	class company_group{
		public:
		string equity_symbol;
		struct stock{
			stock(unsigned int id, unsigned int t, string c, 
						 unsigned int p, unsigned int q, int d) : 
						ID(id), time(t), client(c), 
						price(p), quantity(q), duration(d) {};
			unsigned int ID;
			unsigned int time;
			string client;
//			buy_or_sell action;
			unsigned int price;
			unsigned int quantity;
			int duration;
		};

		public:
		// Comparators
		struct sell_comp{
			bool operator()(const stock& lhs, const stock& rhs) const{
				if(lhs.price < rhs.price) return true;
				else if(lhs.price > rhs.price) return false;
				else if(lhs.ID < rhs.ID) return true;
				else if(lhs.ID > rhs.ID) return false;
				else return false; // Should never be hit.
			}
		} SELL_COMP;
		struct buy_comp{
			bool operator()(const stock& lhs, const stock& rhs) const{
				if(lhs.price > rhs.price) return true;
				else if(lhs.price < rhs.price) return false;
				else if(lhs.ID < rhs.ID) return true;
				else if(lhs.ID > rhs.ID) return false;
				else return false; // Should never be hit.		
			}
		} BUY_COMP;
		
		// THE LIST OF STOCKS 
		// Shall always be sorted lowest->greatest by stock price
		std::deque<company_group::stock> sell_offers;
		// Shall always be sorted greatest->lowest, by stock price
		std::deque<company_group::stock> buy_offers;
		std::vector<stock> stocks_traded;

		void clear_stocks(int current_timestamp);
		void process_stock(buy_or_sell a, int id, int t, string c, int p, int q, int d);

		company_group(string sym) : equity_symbol(sym) {};
		company_group() {std::cout << "DEFAULT CONSTRUCTOR REACHED: YOU HAVE AN ERROR\n";};
	};
	
	// Comparator
	struct string_comp{
		bool operator()(const string& lhs, const string& rhs) const{
			return lhs.compare(rhs) < 0;
		}
	};

	// There will be only one company_group per equity_symbol
	std::map <string, market_maker::company_group, string_comp> placed_orders;
	
	// Flags
	bool median_flag, midpoint_flag;
	void print_median(){}; // NOT YET IMPLEMENTED
	void print_midpoint(){}; // NOT YET IMPLEMENTED
	
	static unsigned int commission;
	bool get_input();
	void update_time(int new_timestamp);
	int current_timestamp;
	unsigned int current_id;
};

unsigned int market_maker::commission = 0;

// Process a line of input - returns false if we're of output, true otherwise
// (or exits)
bool market_maker::get_input(){
	int timestamp, price, quantity, duration;
	string client_name, equity_symbol;
	buy_or_sell action;
	
	string dummy_input;
	char dummy_symbol;

	if(std::cin.eof()) return false;

	// Get timestamp
	std::cin >> dummy_input;
	if(std::cin.eof()) return false;
	for(int i = 0; i < dummy_input.size(); i++)
		if(!std::isdigit(dummy_input[i])) exit(1);
	timestamp = std::stoi(dummy_input);
	if(timestamp < current_timestamp) exit(1);
	update_time(timestamp); // NOT YET IMPLEMENTED
	
	// Get client_name
	std::cin >> dummy_input;
	if(std::cin.eof()) exit(1);
	for(int i = 0; i < dummy_input.size(); i++)
		if(!(std::isalnum(dummy_input[i]) || dummy_input[i] == '_')) exit(1);
	client_name = dummy_input;

	// Get buy_or_sell
	std::cin >> dummy_input;
	if(std::cin.eof()) exit(1);
	if(!dummy_input.compare("BUY"))
		action = BUY;
	else if(!dummy_input.compare("SELL"))
		action = SELL;
	else
		exit(1);

	// Get equity_symbol
	std::cin >> dummy_input;
	if(std::cin.eof()) exit(1);
	for(int i = 0; i < dummy_input.size(); i++)
		if(!(std::isalnum(dummy_input[i]) 
				|| dummy_input[i] == '_' 
				|| dummy_input[i] == '.')) 
			exit(1);
	equity_symbol = dummy_input;
	
	// Get price
	std::cin >> dummy_symbol;
	if(std::cin.eof()) exit(1);
	if(!(dummy_symbol == '$')) exit(1);
	if(std::cin.peek() == std::char_traits<char>::eof()) exit(1);
	std::cin >> dummy_input;
	for(int i = 0; i < dummy_input.size(); i++)
		if(!std::isdigit(dummy_input[i])) exit(1);
	price = std::stoi(dummy_input);
	
	// Get quantity
	std::cin >> dummy_symbol;
	if(std::cin.eof()) exit(1);
	if(!(dummy_symbol == '#')) exit(1);
	if(std::cin.peek() == std::char_traits<char>::eof()) exit(1);
	std::cin >> dummy_input;
	for(int i = 0; i < dummy_input.size(); i++)
		if(!std::isdigit(dummy_input[i])) exit(1);
	quantity = std::stoi(dummy_input);

	// Get duration
	std::cin >> dummy_input;
	if(std::cin.eof()) exit(1);
	for(int i = 0; i < dummy_input.size(); i++)
		if(!std::isdigit(dummy_input[i])) exit(1);
	duration = std::stoi(dummy_input);

	string e = equity_symbol;
	if(placed_orders.find(e) == placed_orders.end())
		placed_orders.insert(std::pair<string, company_group>(e, company_group(e)));
	
	// Hand the reigns over to that stock processor, yo
	placed_orders[e].process_stock(action, current_id, timestamp, client_name, 
																 price, quantity, duration);
	current_id++;
	return true;
}

// Update time and print any median/midpoint options
// Also, cleans out expired orders
void market_maker::update_time(int new_timestamp){
	if(new_timestamp == current_timestamp) return;
	
	if(median_flag) print_median();
	if(midpoint_flag) print_midpoint();
	current_timestamp = new_timestamp;

	// Clear stocks	
	for(auto it = placed_orders.begin(); it != placed_orders.end(); it++)
		it->second.clear_stocks(current_timestamp);
}

// Goes through and checks the duration of each order, deleting as necessary
void market_maker::company_group::clear_stocks(int current_timestamp){
	for(int i = 0; i < buy_offers.size(); i++)
		if(buy_offers[i].duration != -1 
			 && buy_offers[i].time + buy_offers[i].duration <= current_timestamp)
			buy_offers.erase(buy_offers.begin() + i);
	for(int i = 0; i < sell_offers.size(); i++)
		if(sell_offers[i].duration != -1 
			 && sell_offers[i].time + sell_offers[i].duration <= current_timestamp)
			sell_offers.erase(sell_offers.begin() + i);
}

// The workhorse
void market_maker::company_group::process_stock(buy_or_sell a, int id, int t, string c,
																								int p, int q, int d){
//std::cout << "--IN process_stock--\n";
	stock current_stock = stock(id, t, c, p, q, d);
	
	std::deque<stock>* offer_list_dummy_pointer = 0;

	if(a == BUY) offer_list_dummy_pointer = &sell_offers;
	else offer_list_dummy_pointer = &buy_offers;
	
	std::deque<stock>& offer_list = *offer_list_dummy_pointer;

//if(offer_list.empty()) std::cout << "offer_list empty\n";
//std::cout << "current_stock quantity = " << current_stock.quantity << std::endl;
//std::cout << "...just before while loop...\n";
	while(!offer_list.empty() && current_stock.quantity > 0){
//std::cout << "...in while loop...\n";
		// If we're looking to buy/sell more than (slash as much as) they want 
		// to sell/buy
		stock& trade = offer_list.front();
		
		if(a == BUY){
			if(trade.price > current_stock.price){
std::cout << "BUYING:\ncurrent_stock.price:\t" << current_stock.price << std::endl;
std::cout << "trade.price\t" << trade.price << std::endl;
std::cout << "breaking.\n";
				 break;
			}
		}
		else{
			if(trade.price < current_stock.price){
std::cout << "SELLING:\ncurrent_stock.price:\t" << current_stock.price << std::endl;
std::cout << "trade.price\t" << trade.price << std::endl;
std::cout << "breaking.\n";
				break;
			}
		}

std::cout << "Client " << current_stock.client;
if(a == BUY) std::cout << " buying ";
else std::cout << " selling ";
		
		// If we're going to clear out the outstanding offer in the trade
		if(current_stock.quantity >= trade.quantity){
			commission += (trade.quantity*trade.price)/100;
			commission += (trade.quantity*trade.price)/100;
std::cout << trade.quantity << " shares of " << equity_symbol << " to " << trade.client << " @ $";
std::cout << trade.price << "/share for a total commission of $" << 2*((int)(trade.quantity*trade.price)/100);
std::cout << std::endl;
			current_stock.quantity -= trade.quantity;
			stocks_traded.push_back(trade);
			offer_list.pop_front();
		}
		// Or, if we're going to be cleared out by the trade
		else{
			commission += (trade.price*current_stock.quantity)/100;
			commission +=	(trade.price*current_stock.quantity)/100;
std::cout << current_stock.quantity << " shares of " << equity_symbol << " to " << trade.client << " @ $";
std::cout << trade.price << "/share for a total commission of $" << 
					2*((int)(current_stock.quantity*trade.price)/100);
std::cout << std::endl;
			trade.quantity -= current_stock.quantity;
			stocks_traded.push_back(current_stock);
			current_stock.quantity = 0;
		}
	}
	
	// At this point, we should have matched our current_stock with all 
	// possible trades.
	
	// IOC ORDERS
	if(current_stock.duration == 0) return;
	
	if(current_stock.quantity > 0){
		//INSERT CURRENT_STOCK INTO OFFER_LIST
		//..i think?...(need comp)
		if(a == BUY){
//			auto index = std::lower_bound(buy_offers.begin(), buy_offers.end(), current_stock, buy_comp());
			auto index = buy_offers.begin();
			while(index != buy_offers.end()){
				if(BUY_COMP(current_stock, *index)) break;
				index++;
			}
			buy_offers.insert(index, current_stock);
		}
		else{
//			auto index = std::lower_bound(sell_offers.begin(), sell_offers.end(), current_stock, sell_comp);
			auto index = sell_offers.begin();
			while(index != sell_offers.end()){
				if(SELL_COMP(current_stock, *index)) break;
				index++;
			}
		
			sell_offers.insert(index, current_stock);
		}
	}
}

int main(){
	market_maker market;
	while(market.get_input()) {};
	return 0;
}













/*void market_maker::push_stock(string e, int t, string c, buy_or_sell a, 
															int p, int q, int d)	
	if(placed_orders.find(e) == placed_orders.end())
		placed_orders.insert(std::pair<string, company_group>(e, company_group(e)));
	placed_orders[e].process_stock(t, c, a, p, q, d);

//	placed_orders[e].stock_list.emplace_back(t, c, a, p, q, d);

std::cout << placed_orders[e].stock_list.back().time << " ";
std::cout << placed_orders[e].stock_list.back().client << " ";
if(placed_orders[e].stock_list.back().action == BUY) std::cout << "BUY ";
else std::cout << "SELL ";
std::cout << placed_orders[e].equity_symbol << " $";
std::cout << placed_orders[e].stock_list.back().price << " #";
std::cout << placed_orders[e].stock_list.back().quantity << " ";
std::cout << placed_orders[e].stock_list.back().duration << "\n";
}*/






