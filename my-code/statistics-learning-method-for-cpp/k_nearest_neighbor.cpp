#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
using namespace std;
/*
this method is used for reading csv file to a vector class 
*/
vector<string> ReadCsvFile(string path)
{
    ifstream fin (path);
    string my_str;
    vector<string> csv_data;
    while (getline(fin,my_str))
    {
        csv_data.push_back(my_str);
    }
    fin.close();
    return csv_data;
}

/*
This method is used to seperate the data to several array.
It used two dimentions pointer to realize the multidimention array.
*/
vector<vector<string>> DataPreHandle(vector<string> data,int data_row)
{
    vector<vector<string>> my_data;
    vector<string> data_bin;
    string str;
    for(int row=1;row<=data_row;row++)  //get the second line 
    {
        int index=0; 
        for(int j=0;j<data[row].size();j++)
        {
            if (data[row][j]!=',')
            {
                str = str+data[row][j];
            }
            else
            {
                data_bin.push_back(str);
                index++;
                str = "";
            }           
            if(j==data[row].size()-1) 
            {
                data_bin.push_back(str);
                index++;
                str = "";
            }
        }        
        my_data.push_back(data_bin);
        data_bin.clear();
    }
    return  my_data;
}
/*
this method is k nearest neighbor algorithm
*/
void KNN(string path)
{
    
}

int main()
{

}