#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <stdlib.h>

enum buy_or_sell{BUY, SELL};

using std::string;

class market_maker{
	public:
	market_maker() {current_timestamp = 0;}
	// Each one of these structures will hold one particular type of equity, in a
	// vector of "stocks"
	class company_group{
		public:
		string equity_symbol;
		struct stock{
			stock(unsigned int t, string c, buy_or_sell a, 
						unsigned int p, unsigned int q, int d) : 
						time(t), client(c), action(a), 
						price(p), quantity(q), duration(d) {};
			unsigned int time;
			string client;
			buy_or_sell action;
			unsigned int price;
			unsigned int quantity;
			int duration;
		};
		public:
		std::vector<company_group::stock> stock_list;
		std::vector<stock> stocks_traded;
		company_group(string sym) : equity_symbol(sym) {};
		company_group() {std::cout << "DEFAULT CONSTRUCTOR REACHED: YOU HAVE AN ERROR\n";};
	};
	
	struct string_comp{
		bool operator()(const string& rhs, const string& lhs) const{
			return rhs.compare(lhs) < 0;
		}
	};
	//There will be only one company_group per equity_symbol
	std::map <string, market_maker::company_group, string_comp> placed_orders;
	bool get_input();
	void push_stock(string e, int t, string c, buy_or_sell a, int p, int q, int d);
	int current_timestamp;
};

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
	current_timestamp = timestamp;
	
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

	push_stock(equity_symbol, timestamp, client_name, 
						 action, price, quantity, duration);

	return true;
}

void market_maker::push_stock(string e, int t, string c, buy_or_sell a, 
															int p, int q, int d){
	
//	placed_orders.emplace(e, e);
	if(placed_orders.find(e) == placed_orders.end())
		placed_orders.insert(std::pair<string, company_group>(e, company_group(e)));
	placed_orders[e].stock_list.emplace_back(t, c, a, p, q, d);

/*std::cout << placed_orders[e].stock_list.back().time << " ";
std::cout << placed_orders[e].stock_list.back().client << " ";
if(placed_orders[e].stock_list.back().action == BUY) std::cout << "BUY ";
else std::cout << "SELL ";
std::cout << placed_orders[e].equity_symbol << " $";
std::cout << placed_orders[e].stock_list.back().price << " #";
std::cout << placed_orders[e].stock_list.back().quantity << " ";
std::cout << placed_orders[e].stock_list.back().duration << "\n";*/
}


int main(){
	market_maker market;
	while(market.get_input()) {};
	return 0;
}


















