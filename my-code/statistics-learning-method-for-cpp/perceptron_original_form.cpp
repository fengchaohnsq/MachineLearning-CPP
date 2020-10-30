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
string** DataPreHandle(vector<string> data,int data_row,int data_col)
{
    static string ** my_data= new string *[data_row];   // need to declare a static pointer space avoiding the system auto releasing  memory
    string str;
    string col_container[data_col];
    
    for(int row=0;row<data_row;row++)  //split string by comma,by getting string length of each column
    {
        my_data[row] = new string [data_col];  //assign row pointer space for each column
        int index=0; 
        for(int j=0;j<data[row].size();j++)
        {
            if (data[row][j]!=',')
            {
                str = str+data[row][j];
            }
            else
            {
                col_container[index] = str; 
                index++;
                str = "";
            }           
            if(j==data[row].size()-1) 
            {
                col_container[index] = str; 
                index++;
                str = "";
            }
        }        
        for(int col=0;col<data_col;col++)
        { 
            my_data[row][col] = col_container[col];
        }
    }
    return  my_data;
}
/*
This method is  original perceptron algorithm
*/
double PerceptronOriginal(string path, int data_row, int data_column, double learning_rate=0.1,double b=0,double train_data_percent=0.7)// parameters without default value has to be put at the first position.
{
    //initialize weight as 1 and bias as 0
    double bias = 0;
    double[] weight = new double[data_column-1];
    for (int i = 0; i < data_column-1; i++)
    {
        weight[i] = 1;
    }
    //Spliting data as train and prediction data
    vector <string> iris_data = ReadCsvFile(path);
    string** my_data = DataPreHandle(iris_data,data_row,data_column);
    string[][] train_data = new string[data_row*train_data_percent][data_column];
    string[][] test_data = new string[data_row*(1-train_data_percent)][data_column];
    for(int column =0;column<data_column;column++)
    {
        for (int row =0;row<data_row*train_data_percent;row++)
        {
            train_data[row][column]=my_data[row][column];
        }
        for (int row =data_row*train_data_percent;row<data_row;row++)
        {
            test_data[row][column]=my_data[row][column];
        }
    }
    //sign function
    double y = SignFunc(weight,bias,train_data);
    return 0.01;
}
/*
this method is sign function
*/
double SignFunc(double[] weight,double bias,string[][] train_data)
{
    int train_data_lines=sizeof(train_data)/sizeof(train_data[0][0]);
    int train_data_row= sizeof(train_data)/sizeof(train_data[0])
    int train_data_column=train_data_lines/train_data_row;
    for(int row =0 ; row<train_data_row;row++)
    {
        double y=0.0;
        for(int column=0;column<data_column-1;column++)
        {
            y=y+weight[column]*train_data[row][column];
        }
    }
    y=y+bias;
    return y;
}
/*
This method is gradient descent
*/
void RandomGradientDescent()
{}
/*
this method is to predict result
*/

/*
This is main method
*/
int main()
{
    return 0;
}
