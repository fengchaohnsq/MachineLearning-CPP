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
/*
This method used to calculate the distance between two instances. 
It include two types of distances:L1 Euclidean distance,L2 Manhattan distance
*/
double LDistance(vector<double> x_i,vector<double>x_j,double p=1)
{
    double distance=0.0;
    if(x_i.size()==x_j.size())
    {
        for(int i=0;i<x_i.size();i++)
        {
            distance=distance+pow((abs(x_i[i]-x_j[i])),p);
        }
    }
    double result = pow(distance,1/p);
    return result;
}
/*
this method used for sorting result
*/
vector<int> sort_data(vector<double> distance)
{
    vector<int> index;
    for(int num=1;num<distance.size()+1;num++)
    {
        index.push_back(num);
    }
    for(int i=0;i<distance.size();i++)
    {   
        for(int j=i+1;j<distance.size();j++)
        {
            if(distance[i]>distance[j])
            {
                int tem = index[i];
                index[i] = index[j];
                index[j] = tem;
            }
        }
    }
    return index;
}
/*
this method is k nearest neighbor algorithm
*/
double KNN(string train_path,string test_path,int tr_data_row,int te_data_row,int data_column,int k_value)
{
    vector<string> csv_tr_data=ReadCsvFile(train_path);
    vector<vector<string>> my_train_data=DataPreHandle(csv_tr_data,tr_data_row);
    vector<vector<double>> x_train_data;
    vector<double> y_train_data;
    vector<string> csv_te_data=ReadCsvFile(test_path);
    vector<vector<string>> my_test_data=DataPreHandle(csv_te_data,te_data_row);
    vector<vector<double>> x_test_data;
    vector<double> y_test_data;
    vector<double> data_bin;
    //set X train dataset and convert species Y label as 1,2,3
    for (int row =0;row<tr_data_row;row++)
    {
        for(int column =1;column<data_column;column++)// get from second column
        {
            data_bin.push_back(atof(my_train_data[row][column].c_str()));  //c_str is needed to convert string to const char* previously (the function requires it)
        }
        x_train_data.push_back(data_bin);
        data_bin.clear();
        if(my_train_data[row][data_column]=="Iris-setosa")
        {
            y_train_data.push_back(1);
        }
        else if(my_train_data[row][data_column]=="Iris-versicolor")
        {
            y_train_data.push_back(2);
        } 
        else if(my_train_data[row][data_column]=="Iris-virginica")
        {
            y_train_data.push_back(3);
        } 
    }
    //set X test dataset
    for (int row =0;row<te_data_row;row++)
    {
        for(int column =1;column<data_column;column++)// get from second column
        {
            data_bin.push_back(atof(my_test_data[row][column].c_str()));  //c_str is needed to convert string to const char* previously (the function requires it)
        }
        y_test_data.push_back(atof(my_test_data[row][data_column].c_str()));
        x_test_data.push_back(data_bin);
        data_bin.clear(); 
    }
    // calculate the Euclidean distance
    vector<vector<double>> l2_distance;
    vector<double> distance_bin;
    for (int row =0 ; row<x_test_data.size();row++)
    {
        for (int row1 =0 ; row1<x_train_data.size();row1++)
        {
            distance_bin.push_back(LDistance(x_test_data[row],x_train_data[row1],1));  
        }
        l2_distance.push_back(distance_bin);
        distance_bin.clear();
    }
    // sort the result and vote for the result
    vector<double> mistake_rate;
    for(int row=0;row<l2_distance.size();row++)
    {
        vector<int> index=sort_data(l2_distance[row]);
        int wrong_count=0;
        for(int k =0;k<k_value;k++)
        {
            if(y_train_data[index[k]]!=y_test_data[row])
            {
                wrong_count++;
            }
        }
        mistake_rate.push_back(wrong_count/k_value);
    }
   double rate=0.0;
   for(int i=0;i<mistake_rate.size();i++)
   {
       rate=rate+mistake_rate[i];
   }
   return rate/mistake_rate.size();
}

int main()
{
    string train_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris.csv";
    string test_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris_test.csv";
    int tr_data_row=150;
    int te_data_row=15;
    int data_column=5;
    int k_value=12;
    double rate = KNN(train_path,test_path,tr_data_row,te_data_row,data_column,k_value);
    cout<<"using k value as "<<k_value<<" the mistake rate was"<<rate<<endl;
    cout<<"5 zhengchu 2"<<2/5;
}