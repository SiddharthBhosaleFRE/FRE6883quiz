#include <stdio.h>
#include <cstring>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <iomanip>
#include <fstream>
#include "curl/curl.h"

using namespace std;
// const char* cIWB3000SymbolFile = "Russell_3000_component_stocks.csv";

// void populateSymbolVector(vector<string>& symbols)// takes reference to a vector... meaning it can modify the vector passed to it 
// {
// 	ifstream fin; // ifstream fin; declares an object fin of type ifstream (input file stream). This object is used to read data from files.
// 	fin.open(cIWB3000SymbolFile, ios::in); // The ios::in flag specifies that the file is opened in read mode.

// 	string line, name, symbol; 
// 	while (!fin.eof()) // end of the file (eof)
// 	{
// 		//fin >> line;
// 		getline(fin, line); // getline(fin, line); reads a line from fin and stores it in line.
// 		stringstream sin(line); //Constructs a stringstream object sin from the string line.
// 		getline(sin, symbol, ',');
// 		getline(sin, name);
// 		symbols.push_back(symbol);
// 	}
// }

int write_data(void* ptr, int size, int nmemb, FILE* stream)
/*The function write_data is declared with four parameters:
void* ptr: A pointer to the data that has been received. It's a void pointer, meaning it can point to data of any type.
int size: The size (in bytes) of each data element that ptr points to.
int nmemb: The number of data elements that ptr points to.
FILE* stream: A pointer to a FILE object. This is the file where the data is to be written.

// */
{
	size_t written; // Declares a variable written of type size_t, which is used to store the number of items successfully written to the file.
	written = fwrite(ptr, size, nmemb, stream); //It writes nmemb items of data, each size bytes long, from the memory block pointed to by ptr to the given output stream stream.
	return written;
}
struct MemoryStruct {
	char* memory;
	size_t size;
};
void* myrealloc(void* ptr, size_t size)// function with a return as a pointer
{
	if (ptr) // This if statement checks if the ptr is not NULL. In other words, it checks if ptr already points to an allocated memory block.
		return realloc(ptr, size);
	else
		return malloc(size); //If ptr is NULL, malloc is used to allocate a new block of memory of size size. This is equivalent to calling malloc for the first time.
}
int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)data;
	//Casts the void* data pointer to a pointer of type struct MemoryStruct*. This assumes that the user has passed a pointer to a MemoryStruct object. 
	//This struct keeps track of the dynamically allocated memory and its size.
	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);// pointer to a char is assigned with the address of newly allocated memory.
	//Calls myrealloc to resize the memory block pointed to by mem->memory. The new size is the current size plus the size of the 
	//new data (realsize) plus one byte for a null terminator. The memory is cast to char* because the data handled is typically character data.
	if (mem->memory) { //If mem->memory is not NULL, the memory has been successfully reallocated.
		memcpy(&(mem->memory[mem->size]), ptr, realsize); //Copies the new data into the memory block. The copy starts at the end of the current data (mem->memory[mem->size]).
		mem->size += realsize;//Updates the size of the data stored in the memory block.
		mem->memory[mem->size] = 0;//Adds a null terminator at the end of the new data. This is important if the data is treated as a string.
	}
	return realsize;
}


int main(void)
{
    
    ofstream fout;
    fout.open("quiz3.txt");
	// vector<string> symbolList;
	// populateSymbolVector(symbolList);
	string symbol;
	symbol = "MSFT";
	// file pointer to create file that store the data  
	FILE* fp;

	const char resultfilename[FILENAME_MAX] = "Results.txt";

	// declaration of an object CURL 
	CURL* handle;

	CURLcode result;

	// set up the program environment that libcurl needs 
	curl_global_init(CURL_GLOBAL_ALL);

	// curl_easy_init() returns a CURL easy handle 
	handle = curl_easy_init();

	// if everything's all right with the easy handle... 
if (handle)
{
    string url_common = "https://eodhistoricaldata.com/api/eod/";
    string start_date = "2023-11-01";
    string end_date = "2023-11-30";
    string api_token = "656a0d9b46f6e1.43726717";  // You must replace this API token with yours

//    string symbol = symbol; // Replace with your actual symbol

    struct MemoryStruct data;
    data.memory = NULL;
    data.size = 0;

    string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
    curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

    // Adding a user agent
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
    fp = fopen(resultfilename, "ab"); // Opens the results file in append mode and writes the current stock symbol to it.
    fprintf(fp, "%s\n", symbol.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);
    result = curl_easy_perform(handle);
    fprintf(fp, "%c", '\n');
    fclose(fp);

    // Check for errors 
    if (result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        return -1;
    }

    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

    // Perform, then store the expected code in result
    result = curl_easy_perform(handle);

    if (result != CURLE_OK)
    {
        // If errors have occurred, tell us what is wrong with result
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        return 1;
    }


    stringstream sData;
    sData.str(data.memory);
    string sValue, sDate, rValue;
    double dValue = 0;
    double closeValue = 0;
    double minValue = 100000.00;
    double maxValue = 0;
    string sMinOpenDate;
    string sMaxCloseDate;

    string line;
    cout << symbol << endl;

    while (getline(sData, line)) {
        size_t found = line.find('-');
        if (found != std::string::npos) {
            cout << line << endl;
            sDate = line.substr(0, line.find_first_of(','));
            line.erase(line.find_last_of(','));
            line.erase(line.find_last_of(','));
            rValue = line.substr(line.find_last_of(',') + 1);
            closeValue = strtod(rValue.c_str(), NULL);

            sValue = line.substr(line.find_first_of(',') + 1);
            dValue = strtod(sValue.c_str(), NULL);

            if (dValue < minValue) {
                minValue = dValue;
                sMinOpenDate = sDate;
            }
            if (closeValue > maxValue) {
                maxValue = closeValue;
                sMaxCloseDate = sDate;
            }

            cout << sDate << " " << std::fixed << ::setprecision(2) << dValue << " "<< closeValue<< endl;
        }
    }

    fout << "symbol = " << symbol << " minvalue = " << minValue << " " << sMinOpenDate << endl;
    fout << "symbol = " << symbol << " maxvalue = " << maxValue << " " << sMaxCloseDate << endl;

        free(data.memory);
        data.size = 0;
}

	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return -1;
	}

	// cleanup since you've used curl_easy_init  
	curl_easy_cleanup(handle);

	// release resources acquired by curl_global_init() 
	curl_global_cleanup();

	return 0;

}