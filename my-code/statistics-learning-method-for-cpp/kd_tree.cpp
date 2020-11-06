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
            distance=distance+pow((abs(x_i[0]-x_j[0])),p);
        }
    }
    double result = pow(distance,1/p);
    return result;
}
/*
tree node
*/
struct tree_node
{
    int data_id=0; // median number data id
    int split_dimention; // x data sort dimention 
    tree_node *left;// the left tree node which is less than x dimention data median
    tree_node *right;// the right tree node which is more than x dimention data median
};
/*
This method used to sort data by some dimention of data row
*/
vector<vector<double>> SortByDimention(vector<vector<double>>x_data,int data_dimention)
{
    for(int row = 0;row<x_data.size();row++)
    {
        for(int row1 = row+1;row1<x_data.size();row1++)
        {
            if(x_data[row][data_dimention] > x_data[row1][data_dimention])
            {
                vector<double> x_data_temp;
                x_data_temp = x_data[row];
                x_data[row]=x_data[row1];
                x_data[row1]=x_data_temp;
            }
        }
    }
    return x_data;
}
/*
This method used for spliting the data by median position
*/
vector<vector<vector<double>>> SplitData(int split_pos,vector<vector<double>> data)
{
    vector<vector<vector<double>>> splited_data;
    if(data.size()>1)
    {
        vector<vector<double>> bin;
        // left tree data
        for(int row=0;row<split_pos;row++)
        {
            bin.push_back(data[row]);
        }
        splited_data.push_back(bin);
        bin.clear();
        // right tree data
        for(int row=split_pos+1;row<data.size();row++)// exclude the split position data
        {
            bin.push_back(data[row]);
        }
        splited_data.push_back(bin);
        bin.clear();
    }
    return splited_data;
}
/*
This method create node by recursion calling
*/
tree_node CreateNode(int split_dimention,vector<vector<double>> data)
{
    tree_node node;
    int feature_num=data[0].size()-1;// this is also k value, excluding the id item 
    vector<vector<double>> sort_data=SortByDimention(data,split_dimention);// sorting data by data dimention;
    vector<vector<vector<double>>> splited_data=SplitData(sort_data.size()/2,sort_data);
    int split_next_dimention=(split_dimention+1)%feature_num;// find next dimention l = j mod k+1, its range is [0,k)
    node.split_dimention=split_dimention;
    int id_pos=sort_data.size()/2;
    node.data_id=sort_data[id_pos][0];
    //assign data to left and right tree
    if(splited_data[0].size()>1){*node.left = CreateNode(split_next_dimention,splited_data[0]);}
    if(splited_data[1].size()>1){*node.right = CreateNode(split_next_dimention,splited_data[1]);}
    return node;
}
/*
This method is to construct KD tree
*/
tree_node ConstructKdTree(string train_path,int tr_data_row)
{
    vector <string> iris_data = ReadCsvFile(train_path);
    vector<vector<string>> my_data = DataPreHandle(iris_data,tr_data_row);
    cout<<"my_data row"<<my_data.size()<<endl;
    cout<<"my_data column"<<my_data[0].size()<<endl;
    vector<vector<double>> x_train_data;
    vector<double> y_train_data;
    vector<double> data_bin;

    //set X dataset and convert Y label as 1,2,3
    for (int row =0;row<tr_data_row;row++)//get row from first line which include the id
    {
        for(int column =0;column<my_data[0].size()-1;column++)
        {
            data_bin.push_back(atof(my_data[row][column].c_str()));  //c_str is needed to convert string to const char* previously (the function requires it)
        }
        x_train_data.push_back(data_bin);
        data_bin.clear();
        // set Iris-setosa as 1, other species as -1
        if(my_data[row][my_data[0].size()-1]=="Iris-setosa")
        {
            y_train_data.push_back(1);
        }
        else if(my_data[row][my_data[0].size()-1]=="Iris-versicolor")
        {
            y_train_data.push_back(2);
        } 
        else if(my_data[row][my_data[0].size()-1]=="Iris-virginica")
        {
            y_train_data.push_back(3);
        }  
    }
    //construct kd tree
    tree_node root = CreateNode(0,x_train_data);
    return root;
}
/*
This method is preorder travesal B-tree
*/
void Preorder(tree_node node)
{
    Preorder(*node.left);
    Preorder(*node.right);
    cout<<"tree node id:"<<node.data_id<<endl;
}

int main()
{
    string train_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris.csv";
    string test_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris_test.csv";
    int tr_data_row=150;
    int te_data_row=15;
    int data_column=5;
    int k_value=12;
    tree_node root = ConstructKdTree(train_path,tr_data_row);
    Preorder(root);
}