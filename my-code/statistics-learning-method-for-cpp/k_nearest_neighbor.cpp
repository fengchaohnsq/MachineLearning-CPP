#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<math.h>
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
double LDistance(vector<double> x_i,vector<double>x_j,int p=1)
{
    double distance=0.0;
    if(x_i.size()==x_j.size())
    {
        for(int i=0;i<x_i.size();i++)
        {
            distance=distance+pow((abs(x_i[0]-x_j[0])),p);
        }
    }
    return pow(distance,1/p);
}
/*
this method is k nearest neighbor algorithm
*/
void KNN(string path,int data_row, int data_column)
{
    vector<string> csv_data= ReadCsvFile(path);
    vector<vector<string>> my_data= DataPreHandle(csv_data,data_row);
    vector<vector<double>> x_train_data;
    vector<double> y_train_data;
    vector<double> data_bin;

    //set X dataset and convert species Y label as 1,2,3
    for (int row =0;row<(data_row);row++)
    {
        for(int column =1;column<data_column;column++)// get from second column
        {
            data_bin.push_back(atof(my_data[row][column].c_str()));  //c_str is needed to convert string to const char* previously (the function requires it)
        }
        x_train_data.push_back(data_bin);
        data_bin.clear();
        if(my_data[row][data_column]=="Iris-setosa")
        {
            y_train_data.push_back(1);
        }
        else if(my_data[row][data_column]=="Iris-versicolor")
        {
            y_train_data.push_back(2);
        } 
        else if(my_data[row][data_column]=="Iris-virginica")
        {
            y_train_data.push_back(3);
        } 
    }

}

int main()
{

}