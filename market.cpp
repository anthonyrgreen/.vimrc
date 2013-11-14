#include <iostream>
#include <deque>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>

enum buy_or_sell{BUY, SELL};

using std::string;

class market_maker{
	// Each one of these structures will hold one particular type of equity, in a
	// vector of "stocks"
	class company_group{
		// Structure for holding individual orders
		struct stock{
			stock(unsigned int id, unsigned int t, string c, 
						 unsigned int p, unsigned int q, int d) : 
						super(x), ID(id), time(t), client(c), 
						price(p), quantity(q), duration(d) {};
			unsigned int ID, time, price, quantity;
			string client;
			int duration;
		};
		
		// Comparators
		struct sell_comp{
			bool operator()(const stock& lhs, const stock& rhs) const{
				if(lhs.price < rhs.price || (lhs.price == rhs.price && lhs.ID < rhs.ID))
					return true;
				else return false;
			}
		} SELL_COMP;
		struct buy_comp{
			bool operator()(const stock& lhs, const stock& rhs) const{
				if(lhs.price > rhs.price || (lhs.price == rhs.price && lhs.ID < rhs.ID)) 
					return true;
				else return false;	
			}
		} BUY_COMP;

		// Data members:		
		market_maker& super;
		string equity_symbol;
		unsigned int amount_spent, num_traded;

		// Outstanding orders
		// Shall always be sorted lowest->greatest by stock price
		std::deque<company_group::stock> sell_offers;
		// Shall always be sorted greatest->lowest, by stock price
		std::deque<company_group::stock> buy_offers;
		// Completed orders
		std::vector<stock> stocks_traded;

		// Methods
		void clear_stocks(unsigned int current_timestamp);
		void process_stock(buy_or_sell a, int id, int t, 
											 string c, int p, int q, int d);
		void print_verbose(string buyer, string seller,	
											 unsigned int p, unsigned int q);
		void print_median();
		void print_midpoint();
		void print_VWAP();

		public:	
		// Constructors
		company_group(market_maker& x, string sym) : super(x), equity_symbol(sym),
																								 amount_spent(0), num_traded(0) {};
		// REMOVE BEFORE TURNING IN
		company_group() {std::cout << "DEFAULT CONSTRUCTOR REACHED: YOU HAVE AN ERROR\n";};
	};
	struct client{
		client() {};
		client(string client_name) : name(client_name), stocks_bought(0)
																 stocks_sold(0), value_traded(0) {};
		strint name;
		unsigned int stocks_bought, stocks_sold, value_traded;
	};

	// Comparator
	struct string_comp{
		bool operator()(const string& lhs, const string& rhs) const{
			return lhs.compare(rhs) < 0;
		}
	};

	// There will be only one company_group per equity_symbol
	std::map <string, company_group, string_comp> placed_orders;
	// Stupid fucking --transfers option requires that I make this stupid map
	std::map <string, client, string_comp> clients;

	// Flags
	static bool verbose_flag, median_flag, midpoint_flag, transfers_flag;

	market_maker();	
	void print_medians();
	void print_midpoints();
	void print_transfers();
	
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
bool market_maker::median_flag = false;
bool market_maker::midpoint_flag = false;
bool market_maker::verbose_flag = false;
bool market_maker::transfers_flag = false;

market_maker::market_maker() : current_id(0) {
	market_maker::verbose_flag = true;
	market_maker::median_flag = true;
	market_maker::midpoint_flag = true;
	market_maker::transfers_flag = true;
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
		placed_orders.insert(std::pair<string, company_group>(e, company_group(*this, e)));
	
	// Stupid goddamned --transfers option
	if(transfers_flag)
		if(clients.find(client_name) == clients.end())
			clients.insert(std::pair<string, client>(client_name, client(client_name)));
	
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

// PRINTING FUNCTIONS
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
		else{
			int left = stocks_traded[(int)((size - 1) / 2)].price;
			int right = stocks_traded[size / 2].price;
			median = (left + right)/2;
		}
		std::cout << "Median match price of " << equity_symbol << " at time " 
							<< super.current_timestamp << " is $" << median << std::endl;
}
void market_maker::company_group::print_midpoint(){
	if(buy_offers.empty() || sell_offers.empty()){
		std::cout << "Midpoint of " << equity_symbol << " at time " 
							<< current_timestamp << " is undefined" << std::endl;
		return;
	}
	int midpoint = (buy_offers.front().price +  sell_offers.front().price) / 2;
	std::cout << "Midpoint of " << equity_symbol << " at time " 
						<< current_timestamp << " is $" << midpoint << std::endl;
}
void market_maker::print_transfers(){
	for(auto i = clients.begin; i != clients.end(); i++){
		client& dude = i->second;
		std::cout << dude.name << " bought " << dude.stocks_bought << " and sold "
							<< dude.stocks_sold << " for a net transfer of $" << dude.value_traded
							<< std::endl;
}
void market_maker::print_VWAP(){
	num_traded
}
void market_maker::end_of_day(){
	std::cout << "---End of Day---\n";
	std::cout << "Commission Earnings: $" << commission << std::endl;
	std::cout << "Total Amount of Money Transferred: $" 
						<< money_transferred << std::endl;
	std::cout << "Number of Completed Trades: " << completed_trades << std::endl;
	std::cout << "Number of Shares Traded: " << shares_traded << std::endl;
	if(verbose_flag) print_transfers();
}

// The workhorse
void market_maker::company_group::process_stock(buy_or_sell a, int id, int t, 
																								string c, int p, int q, int d){
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
		
		unsigned int quantity, value;
		// If we're going to clear out the outstanding offer in the trade
		if(current_stock.quantity >= trade.quantity){
			quantity = trade.quantity;
			value = trade.price * trade.quantity;
		}
		// Or if we're going to be cleared out in the trade
		else{
			quantity = current_stock.quantity;
			value = trade.price * current_stock.quantity;
		}

		if(super.verbose_flag){
			if(a == BUY)
				print_verbose(current_stock.client, trade.client, 
											trade.price, quantity);
			else
				print_verbose(trade.client, current_stock.client, 
											trade.price, quantity);
		}
		if(transfers_flag){
			super.clients[current_stock.client].value_traded += value;
			if(a == BUY)
				super.clients[current_stock.client].stocks_bought += quantity;
			else
				super.clients[current_stock.client].stocks_sold += quantity;
		}
		if(VWAP_flag){
			num_traded += quantity;
			amount_spent += value;
		}

		commission += 2*((int)(value)/100);
		money_transferred += value;
		shares_traded += quantity;
			
		current_stock.quantity -= quantity;
		
		if(current_stock.quantity >= trade.quantity){
			stocks_traded.push_back(trade);
			offer_list.pop_front();
		}
		else{
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



