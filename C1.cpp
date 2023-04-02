#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

#define START_TIME_IN_MINUTES 480
#define FILE_NAME_WITHOUT_SLASH_AND_DOT_DISTANCE 2
#define LOCATION_NAME_INDEX 0
#define START_TIME_INDEX 1
#define END_TIME_INDEX 2
#define RANK_INDEX 3
#define MIN_VISIT_TIME_DURATION 15
#define OPTIMUM_VISIT_TIME_DURATION 60
#define TRANSPORTATION_DURATION 30
#define MINUTES_PER_HOURS 60
#define FALSE -1
#define TIME_DELIMETER ':'
#define ENTRY_DELIMETER ','
#define PLANS_DELIMETER "---"

struct Locations
{
	int number;
	string name;
	int opening_time;
	int closing_time;
	int rank;
};

string get_first_line(string file_name)
{
	ifstream file(file_name);
	string temp_str;
	getline(file, temp_str);
	return temp_str;
}

vector<string> split_first_line(string first_line)
{
	vector<string> titles;
	string token;
	stringstream S(first_line);
	while (getline(S, token, ENTRY_DELIMETER))
		titles.push_back(token);
	return titles;
}

int search(vector<string> vec, string element)
{
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i] == element)
			return i;
	}
	return -1;
}

vector<int> arrange(vector<string> titles)
{
	int name_index = search(titles, "name");
	int opentime_index = search(titles, "openingTime");
	int closetime_index = search(titles, "closingTime");
	int rank_index = search(titles, "rank");
	vector<int> title_arrangment = {name_index, opentime_index, closetime_index, rank_index};
	return title_arrangment;
}

vector<string> read_locs_data(string file_name)
{
	vector<string> input_strings;
	ifstream file(file_name);
	string locations_datas;
	string first_line;
	getline(file, first_line);
	while (getline(file, locations_datas))
		input_strings.push_back(locations_datas);
	return input_strings;
}

vector<vector<string>> split_input(vector<string> input_strings)
{
	int location_number;
	vector<vector<string>> input_table;
	string token;
	location_number = input_strings.size();
	vector<string> temp_vec;
	for (int i = 0; i < location_number; i++)
	{
		stringstream location_data(input_strings[i]);
		while (getline(location_data, token, ENTRY_DELIMETER))
		{
			temp_vec.push_back(token);
		}
		input_table.push_back(temp_vec);
		temp_vec.clear();
	}
	return input_table;
}

vector<int> create_time_vector(vector<vector<string>> input_table, int time_index)
{
	vector<int> times;
	string token;
	vector<int> temp;
	int location_number = input_table.size();
	for (int i = 0; i < location_number; i++)
	{
		stringstream S(input_table[i][time_index]);
		while (getline(S, token, TIME_DELIMETER))
		{
			stringstream ss(token);
			int temp_clock = 0;
			ss >> temp_clock;
			temp.push_back(temp_clock);
		}
		int time = temp[0] * OPTIMUM_VISIT_TIME_DURATION + temp[1];
		temp.clear();
		times.push_back(time);
	}
	return times;
}

vector<Locations> put_input_to_struct(vector<vector<string>> input_table, vector<int> open_times, vector<int> close_times,
 vector<int> title_arrangment)
{
	vector<Locations> input_structs;
	int string_num = input_table.size();
	for (int i = 0; i < string_num; i++)
	{
		input_structs.push_back({i + 1, input_table[i][title_arrangment[LOCATION_NAME_INDEX]], open_times[i], close_times[i],
								 stoi(input_table[i][title_arrangment[RANK_INDEX]])});
	}
	return input_structs;
}

int find_min(vector<int> vec)
{
	vector<string> nearest_times;
	int length = vec.size();
	if (length > 1)
	{
		int tail = vec[length - 1];
		vec.pop_back();
		if (tail <= find_min(vec))
		{
			return tail;
		}
		else
			return find_min(vec);
	}
	else
		return vec[0];
}

int find_int(vector<int> vec, int element)
{
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i] == element)
			return 1;
	}
	return 0;
}
void find_suitable_indexs(vector<Locations> input, int nearest_time, vector<int> location_check, vector<int> &suitable_indexs,
						  vector<int> unsuitable_indexs)
{
	// we find the next suitable indexes and put them in suitable
	// indexes, since we change the vector we pass it with & (i.e. pass by reference) so the changes will apply to the original
	int size = input.size();
	for (int i = 0; i < size; i++)
	{
		if (input[i].opening_time == nearest_time)
		{
			int existence_checker = find_int(location_check, (input[i].number - 1));
			int unsuitable_checker = find_int(unsuitable_indexs, i);
			if (existence_checker == 0 && unsuitable_checker == 0)
				suitable_indexs.push_back(i);
		}
	}
}

int match_num_rank(vector<Locations> input, int rank)
{
	for (int i = 0; i < input.size(); i++)
	{
		if (input[i].rank == rank)
			return i;
	}
	return -1;
}
int find_best(vector<int> suitable_indexs, vector<Locations> input)
{
	vector<int> ranks;
	int index;
	int size3 = suitable_indexs.size();
	for (int i = 0; i < size3; i++)
	{
		index = suitable_indexs[i];
		ranks.push_back(input[index].rank);
	}
	int best = find_min(ranks);
	return best;
}

int calculate(int previous_time, int duration)
{
	if (duration >= OPTIMUM_VISIT_TIME_DURATION)
		duration = OPTIMUM_VISIT_TIME_DURATION;
	return previous_time + duration + TRANSPORTATION_DURATION;
}

int calculate_endtime(int previous_time, int duration)
{
	if (duration >= OPTIMUM_VISIT_TIME_DURATION)
		duration = OPTIMUM_VISIT_TIME_DURATION;
	return previous_time + duration;
}

vector<int> find_open_locs(vector<int> open_times, int current_time, vector<int> location_check, vector<int> unsuitable_indexs)
{
	vector<int> suitable_indexs;
	for (int i = 0; i < open_times.size(); i++)
	{
		int existence_checker = find_int(location_check, i);
		if (open_times[i] <= current_time && existence_checker == 0)
		{
			int unsuitable_check = find_int(unsuitable_indexs, i);
			if (unsuitable_check == 0)
				suitable_indexs.push_back(i);
		}
	}
	return suitable_indexs;
}

int find_nearest(int current_time, vector<int> opentimes)
{
	vector<int> late_opentimes;
	for (int i = 0; i < opentimes.size(); i++)
	{
		if (opentimes[i] > current_time)
		{
			late_opentimes.push_back(opentimes[i]);
		}
	}
	int min = find_min(late_opentimes);
	return min;
}

int find_next_destination_index(int &current_time, vector<int> open_times, vector<Locations> input, vector<int> location_check,
								vector<int> unsuitable_indexs)
{
	vector<int> suitable_indexs;
	int rank, index;
	int counter = 0;
	int len = open_times.size();
	while (true)
	{
		// we stay in this while till suitable indexes is empty
		suitable_indexs.clear();
		if (current_time >= find_min(open_times) && counter < len)
		{
			suitable_indexs = find_open_locs(open_times, current_time, location_check, unsuitable_indexs);
			counter += 1;
		}
		else
		{
			vector<int> late_opentimes;
			for (int i = 0; i < open_times.size(); i++)
			{
				if (open_times[i] > current_time)
					late_opentimes.push_back(open_times[i]);
			}
			int nearest_time = find_min(late_opentimes);
			find_suitable_indexs(input, nearest_time, location_check, suitable_indexs, unsuitable_indexs);
			current_time = nearest_time;
		}
		if (suitable_indexs.size() > 0)
		{
			rank = find_best(suitable_indexs, input);
			index = match_num_rank(input, rank);
			break;
		}
	}
	return index;
}

int check_destination_wellness(vector<Locations> input, int current_time, int index)
{
	int duration = input[index].closing_time - current_time;
	if (duration > MIN_VISIT_TIME_DURATION)
		return duration;
	else
		return -1;
}

int find_max(vector<int> closing_time)
{
	int size = closing_time.size();
	if (size == 1)
		return closing_time[0];
	else
	{
		int tail = closing_time[size - 1];
		closing_time.pop_back();
		if (tail >= find_max(closing_time))
			return tail;
		else
			return find_max(closing_time);
	}
}

int check_existence(vector<int> location_check, int index)
{
	for (int i = 0; i < location_check.size(); i++)
	{
		if (location_check[i] == index)
			return i;
	}
	return -1;
}

void make_changes(int &current_time, int duration_check, int index, vector<int> &location_check, vector<int> &start, vector<int> &durations)
{
	location_check.push_back(index);
	start.push_back(current_time);
	durations.push_back(duration_check);
	current_time = calculate(current_time, duration_check);
}

void test_destinations(int &current_time, vector<Locations> input, vector<int> &location_check, vector<int> &start, vector<int> &durations,
					   vector<int> &not_suitables,vector<int> open_times)
{
	int index = find_next_destination_index(current_time, open_times, input, location_check, not_suitables);
	int existence_checker = check_existence(location_check, index);
	int duration_check = check_destination_wellness(input, current_time, index);
	if (existence_checker == FALSE && duration_check != FALSE)
		make_changes(current_time, duration_check, index, location_check, start, durations);
	else
		not_suitables.push_back(index);
}

void find_next_destination(int current_time, vector<Locations> input, vector<int> &location_check, vector<int> &start, vector<int> &durations,
vector<int> open_times,vector<int> close_times)
{
	// TODO: fix this function
	int size = input.size();
	vector<int> not_suitables;
	int counter = 0;
	while (current_time < find_max(close_times) && counter < size)
	{
		test_destinations(current_time, input, location_check, start, durations, not_suitables,open_times);
		counter += 1;
	}
}

string calculate_correct_clock_form(int hour,int minute,string hour_str,string minute_str)
{
	if (hour < 10 && minute < 10)
		return "0" + hour_str + TIME_DELIMETER + "0" + minute_str;
	else if (hour < 10 && minute >= 10)
		return "0" + hour_str + TIME_DELIMETER + minute_str;
	else if (hour >= 10 && minute < 10)
		return hour_str + TIME_DELIMETER + "0" + minute_str;
	else
		return hour_str + TIME_DELIMETER + minute_str;
}

string convert_int_to_string_stream(int time)
{
	stringstream string_time;
	string_time << time;
	return string_time.str();
}

string convert_int_to_clock_form(int time)
{
	int hour = time / MINUTES_PER_HOURS;
	int minute = time - (hour * MINUTES_PER_HOURS);
	stringstream string_hours;
	string_hours << hour;
	string hour_str = convert_int_to_string_stream(hour);
	string minute_str=convert_int_to_string_stream(minute);
	return calculate_correct_clock_form(hour,minute,hour_str,minute_str);
}

vector<vector<string>> make_vector_ready_for_print(vector<Locations> input, vector<int> close_times, vector<int> &location_check,
												   vector<int> &start, vector<int> &durations)
{
	// in this function we are gonna make the
	// vector ready for print
	vector<vector<string>> final_plan;
	for (int i = 0; i < location_check.size(); i++)
	{
		int end = calculate_endtime(start[i], durations[i]);
		string standard_start = convert_int_to_clock_form(start[i]);
		string standard_end = convert_int_to_clock_form(end);
		vector<string> plan_data = {input[location_check[i]].name, standard_start, standard_end};
		final_plan.push_back(plan_data);
	}
	return final_plan;
}

void print_output(vector<vector<string>> temp_vector)
{
	for (int i = 0; i < temp_vector.size(); i++)
	{
		cout << "Location " << temp_vector[i][LOCATION_NAME_INDEX] << endl
			 << "Visit from " << temp_vector[i][START_TIME_INDEX] << " until " << temp_vector[i][END_TIME_INDEX] << endl
			 << PLANS_DELIMETER << endl;
	}
}

vector<Locations> read_from_file(string file_name,vector<int> &open_times,vector<int> &close_times)
{
	// this function reads input from a file
	string first_line = get_first_line(file_name);
	vector<string> splitted_first_line = split_first_line(first_line);
	vector<int> arrangment = arrange(splitted_first_line);
	vector<string> primitive_get = read_locs_data(file_name);
	vector<vector<string>> splitted_input = split_input(primitive_get);
	open_times = create_time_vector(splitted_input, arrangment[START_TIME_INDEX]);
	close_times = create_time_vector(splitted_input, arrangment[END_TIME_INDEX]);
	vector<Locations> location_data = put_input_to_struct(splitted_input, open_times, close_times, arrangment);
	return location_data;
}

int main(int argc, char *argv[])
{
	vector<int> gone_location;
	vector<int> start_times;
	vector<int> durations;
	vector<int> open_times;
	vector<int> close_times;
	vector<Locations> location_data = read_from_file(argv[1] + FILE_NAME_WITHOUT_SLASH_AND_DOT_DISTANCE,open_times,close_times);
	find_next_destination(START_TIME_IN_MINUTES, location_data, gone_location, start_times, durations,open_times,close_times);
	vector<vector<string>> ready_to_print = make_vector_ready_for_print(location_data, close_times, gone_location, start_times, durations);
	print_output(ready_to_print);
}
