#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<math.h>
#include <algorithm>
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
vector<vector<vector<string>>> DataPreHandle(string path,int data_row)
{
    vector <string> iris_data = ReadCsvFile(path);
    vector<string> data_bin;
    vector<vector<string>> my_data_bin;
    vector<vector<string>> data_bin1;
    vector<vector<vector<string>>> my_data;
    string str;
    for(int row=1;row<=data_row;row++)  //get the second line 
    {
        int index=0; 
        for(int j=0;j<iris_data[row].size();j++)
        {
            if (iris_data[row][j]!=',')
            {
                str = str+iris_data[row][j];
            }
            else
            {
                data_bin.push_back(str);
                index++;
                str = "";
            }           
            if(j==iris_data[row].size()-1) // the last data
            {
                data_bin.push_back(str);
                index++;
                str = "";
            }
        }        
        my_data_bin.push_back(data_bin);
        data_bin.clear();
    }
    //set Y label
    for (int row =0;row<data_row;row++)//get row from first line which include the id
    {
       
        data_bin.push_back(my_data_bin[row][my_data_bin[row].size()-1]);  //get the last data of each row, which is the label data.
        data_bin1.push_back(data_bin);
        data_bin.clear();
    }
    my_data.push_back(data_bin1);
    data_bin1.clear();
    //set X data
    for (int row =0;row<data_row;row++)//get row from first line which include the id
    {
        for(int column =0;column<my_data_bin[row].size()-1;column++)
        {
            data_bin.push_back(my_data_bin[row][column]);  //get the data of each row except the last data,which is the x data.
        }
        data_bin1.push_back(data_bin);
        data_bin.clear();
    }
    my_data.push_back(data_bin1);
    data_bin1.clear();
    return  my_data;
}
/*
This method is used to converse the string type data to double and spliting data to x data and y label.
*/
void StringToDouble (vector<vector<vector<string>>> my_data,vector<string> label_class,vector<vector<double>> &x_data,vector<string> &label)
{
    vector<vector<string>> y_bin=my_data[0];
    vector<vector<string>> x_bin=my_data[1];
    vector<double>data_bin;
    //set X dataset 
    for (int row =0;row<x_bin.size();row++)//get row from first line which include the id
    {
        for(int column =0;column<x_bin[0].size();column++)
        {
            data_bin.push_back(atof(x_bin[row][column].c_str()));  //c_str is needed to convert string to const char* previously (the function requires it)
        }
        x_data.push_back(data_bin);
        data_bin.clear();  
    }
    //set Y label
    for (int row =0;row<x_bin.size();row++)//get row from first line which include the id
    {
        int index=0;
        while (true)
        {
            if(y_bin[row][0]==label_class[index])
            {
                label.push_back(y_bin[row][0]);
                break;
            }
            index++;
        }
    }
}
/*
generating bayes classifier 
P(Y|X)P(X)= P(X|Y)P(Y)=P(XY)
P(Y|X)=P(X|Y)P(Y)/P(X)
Classifirt is to find the bigest P(Y|X),it means to find the biggest P(X|Y)P(Y).
Method will return all of P(Yk),which k is the number of labels, 
and it also returns all of P(Xi|Yk) in k labels, which i is the number of features.
*/
void BayesClassifier(vector<vector<double>> x_data,vector<string> label,vector<string>label_class)
{
    vector<double> P_Yk;
    vector<vector<double>> P_Xi_Yk;
    
 

}

int main()
{
    // setting Y label string
    vector<string> label_class;
    label_class.push_back("Iris-setosa");
    label_class.push_back("Iris-versicolor");
    label_class.push_back("Iris-virginica");
    // data prehandel
    string train_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris.csv";
    string test_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris_test_label.csv";
    int tr_data_row=150;
    vector<vector<double>> x_data;  //train X data
    vector<string> label;// train Y label
    vector<vector<vector<string>>> my_data = DataPreHandle(train_path,tr_data_row);
    StringToDouble(my_data,label_class,x_data,label);
    vector<vector<double>> x_test_data;//test X data
    vector<string> test_label;// test Y lebel
    vector<vector<vector<string>>> my_test_data=DataPreHandle(test_path,15);
    StringToDouble(my_test_data,label_class,x_test_data,test_label);
    
}
