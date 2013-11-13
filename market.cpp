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
	market_maker();
	
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

		void clear_stocks(unsigned int current_timestamp);
		void process_stock(buy_or_sell a, int id, int t, string c, int p, int q, int d);
		void print_verbose(string buyer, string seller,	unsigned int p, unsigned int q);
		void print_median();
		
		company_group(string sym) : equity_symbol(sym) {};
		// REMOVE BEFORE TURNING IN
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
	static bool verbose_flag;

	void print_medians(); // NOT YET IMPLEMENTED
	void print_midpoints(){}; // NOT YET IMPLEMENTED
	
	static unsigned int commission;
	static unsigned int shares_traded;
	static unsigned int money_transferred;
	static unsigned int completed_trades;
	static unsigned int current_timestamp;
	bool get_input();
	void update_time(unsigned int new_timestamp);
	void end_of_day();
	unsigned int current_id;
};

unsigned int market_maker::commission = 0;
unsigned int market_maker::shares_traded = 0;
unsigned int market_maker::completed_trades = 0;
unsigned int market_maker::money_transferred = 0;
unsigned int market_maker::current_timestamp = 0;
bool market_maker::verbose_flag = false;

market_maker::market_maker() : current_id(0) {
	market_maker::verbose_flag = true;
	market_maker::commission = 0;
	market_maker::shares_traded = 0;
	market_maker::completed_trades = 0;
	market_maker::money_transferred = 0;
	market_maker::current_timestamp = 0;
};

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
		if(!std::isdigit(dummy_input[i]))
			 exit(1);
	timestamp = std::stoi(dummy_input);
	if(timestamp < (int)current_timestamp) exit(1);
	update_time((unsigned int)timestamp); // NOT YET IMPLEMENTED
	
	// Get client_name
	std::cin >> dummy_input;
	if(std::cin.eof()) exit(1);
	for(int i = 0; i < dummy_input.size(); i++)
		if(!(std::isalnum(dummy_input[i]) || dummy_input[i] == '_'))
			exit(1);
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
		if(!std::isdigit(dummy_input[i]) && !(i == 0 && dummy_input[i] == '-'))
			 exit(1);
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
void market_maker::update_time(unsigned int new_timestamp){
	if(new_timestamp == current_timestamp) return;
	
	if(median_flag) print_medians();
	if(midpoint_flag) print_midpoints();
	current_timestamp = new_timestamp;

	// Clear stocks	
	for(auto it = placed_orders.begin(); it != placed_orders.end(); it++)
		it->second.clear_stocks(current_timestamp);
}

// Goes through and checks the duration of each order, deleting as necessary
void market_maker::company_group::clear_stocks(unsigned int current_timestamp){
	for(int i = 0; i < buy_offers.size(); i++)
		if(buy_offers[i].duration != -1 
			 && buy_offers[i].time + buy_offers[i].duration <= current_timestamp)
			buy_offers.erase(buy_offers.begin() + i);
	for(int i = 0; i < sell_offers.size(); i++)
		if(sell_offers[i].duration != -1 
			 && sell_offers[i].time + sell_offers[i].duration <= current_timestamp)
			sell_offers.erase(sell_offers.begin() + i);
}

void market_maker::company_group::print_verbose(string buyer, string seller,
																								unsigned int p, unsigned int q){
	std::cout << buyer << " purchased " << q << " shares of " << equity_symbol
						<< " from " << seller << " for $" << p << "/share\n";
}
void market_maker::print_medians(){
	for(auto i = placed_orders.begin(); i != placed_orders.end(); i++)
		i->second.print_median();
}
void market_maker::print_midpoints(){
	for(auto i = placed_orders.begin(); i != placed_orders.end(); i++)
		i->second.print_midpoint();
}
void market_maker::company_group::print_median(){
		if(stocks_traded.empty()) return;
		double median;
		int size = stocks_traded.size();
		if(size % 2)
			median = stocks_traded[(int)size/2].price;
		else
			median = (stocks_traded[size/2].price + stocks_traded[(int)(size-1)/2].price)/2;
		std::cout << "Median match price of " << equity_symbol << " at time " 
							<< market_maker::current_timestamp << " is $" << median << std::endl;
}

void market_maker::end_of_day(){
	std::cout << "---End of Day---\n";
	std::cout << "Commission Earnings: $" << commission << std::endl;
	std::cout << "Total Amount of Money Transferred: $" 
						<< money_transferred << std::endl;
	std::cout << "Number of Completed Trades: " << completed_trades << std::endl;
	std::cout << "Number of Shares Traded: " << shares_traded << std::endl;
}

// The workhorse
void market_maker::company_group::process_stock(buy_or_sell a, int id, int t, string c,
																								int p, int q, int d){
	stock current_stock = stock(id, t, c, p, q, d);
	
	std::deque<stock>* offer_list_dummy_pointer = 0;

	if(a == BUY) offer_list_dummy_pointer = &sell_offers;
	else offer_list_dummy_pointer = &buy_offers;
	
	std::deque<stock>& offer_list = *offer_list_dummy_pointer;

	while(!offer_list.empty() && current_stock.quantity > 0){
		// If we're looking to buy/sell more than (slash as much as) they want 
		// to sell/buy
		stock& trade = offer_list.front();
		
		if(a == BUY){
			if(trade.price > current_stock.price)
				 break;
		}
		else{
			if(trade.price < current_stock.price)
				 break;		
		}

		// If we're going to clear out the outstanding offer in the trade
		if(current_stock.quantity >= trade.quantity){
			if(market_maker::verbose_flag){
				if(a == BUY) 
					print_verbose(current_stock.client, trade.client, trade.price, trade.quantity);
				else
					print_verbose(trade.client, current_stock.client, trade.price, trade.quantity);
			}

			commission += 2*((int)(trade.quantity*trade.price)/100);
			money_transferred += trade.price*trade.quantity;
			shares_traded += trade.quantity;

			current_stock.quantity -= trade.quantity;
			stocks_traded.push_back(trade);
			
			offer_list.pop_front();
		}
		// Or, if we're going to be cleared out by the trade
		else{
			if(market_maker::verbose_flag){
				if(a == BUY) 
					print_verbose(current_stock.client, trade.client, trade.price, current_stock.quantity);
				else
					print_verbose(trade.client, current_stock.client, trade.price, current_stock.quantity);
			}

			commission += 2*((int)(trade.price*current_stock.quantity)/100);
			money_transferred += trade.price*current_stock.quantity;
			shares_traded += current_stock.quantity;

			trade.quantity -= current_stock.quantity;
			stocks_traded.push_back(current_stock);
			
			current_stock.quantity = 0;
		}
		
		completed_trades++;
	}
	
	// At this point, we should have matched our current_stock with all 
	// possible trades. Now it gets inserted into our data structures.
	
	// IOC ORDERS
	if(current_stock.duration == 0) return;

	// Non-IOC ORDERS	
	if(current_stock.quantity > 0){
		//INSERT CURRENT_STOCK INTO OFFER_LIST
		if(a == BUY){
			auto index = buy_offers.begin();
			while(index != buy_offers.end()){
				if(BUY_COMP(current_stock, *index)) break;
				index++;
			}
			buy_offers.insert(index, current_stock);
		}
		else{
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
	market.end_of_day();
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






