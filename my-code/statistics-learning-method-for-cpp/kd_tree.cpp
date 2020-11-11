#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<math.h>
#include <algorithm>
using namespace std;
vector<vector<double>> x_train_data;
vector<vector<double>> origin_x_train_data;
vector<double> y_train_data;
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
It include two types of distances:p =1 is L1 Manhattan distance, p=2 is L2 Euclidean distance.
*/
double LDistance(vector<double> x_i,vector<double>x_j,double p=1,bool is_exclude = true)
{
    double distance=0.0;
    if(x_i.size()==x_j.size()&& is_exclude)
    {
        for(int i=0;i<x_i.size();i++)
        {
            distance=distance+pow((abs(x_i[i]-x_j[i])),p);
        }
    }
    // exclude the id column
    if(x_i.size()==x_j.size()&& !is_exclude)
    {
        for(int i=1;i<x_i.size();i++)
        {
            distance=distance+pow((abs(x_i[i]-x_j[i])),p);
        }
    }
    if(x_i.size()<x_j.size())
    {
        for(int i=0;i<x_i.size();i++)
        {
            distance=distance+pow((abs(x_i[i]-x_j[i+1])),p);
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
    //int data_id; // median number data id
    vector<double> row_data;
    int split_dimention; // x data sort dimention 
    tree_node *left=NULL;// the left tree node which is less than x dimention data median
    tree_node *right=NULL;// the right tree node which is more than x dimention data median
    tree_node *parent=NULL;
    tree_node(vector<double> data,int dimention)
    {
        row_data = data;
        split_dimention= dimention;
        left = NULL;
        right = NULL;
        parent = NULL;
    }
};
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
This method used to sort data by some dimention of data row
*/
void SortByDimention(int left, int right,int data_dimention)
{
    for(int row =left;row<right;row++)
    {
        for(int row1 = left+1;row1<right;row1++)
        {
            if(x_train_data[row][data_dimention] > x_train_data[row1][data_dimention])
            {
                vector<double> x_data_temp;
                x_data_temp = x_train_data[row];
                x_train_data[row]=x_train_data[row1];
                x_train_data[row1]=x_data_temp;
            }
        }
    }
}
/*
This method create node by recursion calling
*/
tree_node* CreateTreeNode(int left,int right, int dim)
{
    if(right<left)
    {
        return NULL;
    }
    int feature_num=x_train_data[0].size()-1;
    int mid = (left + right+1) / 2;
    //sort(x_train_data.begin()+left,x_train_data.begin()+right+1,cmp);
    SortByDimention(left,right,dim+1);// exclude the id column
    tree_node *node =new tree_node(x_train_data[mid],dim);
    node->left=CreateTreeNode(left,mid-1,(dim + 1) % feature_num);
    if(node->left!=NULL)
    {
        node->left->parent=node;
    }
    node->right=CreateTreeNode(mid+1,right,(dim + 1) % feature_num);
    if(node->right!=NULL)
    {
        node->right->parent=node;
    }
    return node;
}
/*
This method is to construct KD tree1
*/
tree_node *root;
void ConstructKdTree(string train_path,int tr_data_row)
{
    vector <string> iris_data = ReadCsvFile(train_path);
    vector<vector<string>> my_data = DataPreHandle(iris_data,tr_data_row);
    
    // cout<<"my_data row"<<my_data.size()<<endl;
    // cout<<"my_data column"<<my_data[0].size()<<endl;
    vector<double> data_bin;

    //set X dataset and convert Y label as 1,2,3
    for (int row =0;row<tr_data_row;row++)//get row from first line which include the id
    {
        for(int column =0;column<my_data[0].size()-1;column++)
        {
            data_bin.push_back(atof(my_data[row][column].c_str()));  //c_str is needed to convert string to const char* previously (the function requires it)
        }
        x_train_data.push_back(data_bin);
        origin_x_train_data.push_back(data_bin);
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
    // cout<<"x_train_data.size():"<<x_train_data.size()<<endl;
    root = CreateTreeNode(0,x_train_data.size()-1,0);
}
/*
This method is travesal  kd tree with LDR or DLR or LRD sequence
*/
void printKdTreeNode(tree_node * root)
{
    if (root == NULL){return;}
    //cout << root->data_id << "\t";//DLR
    printKdTreeNode(root->left);
    cout << root->row_data[0] << "-"; //LDR
    printKdTreeNode(root->right);
    //cout << root->data_id << "\t";//LRD
}
/*
sorting the nearest neighbors list
*/
vector<vector<double>> SortNeighbors(vector<vector<double>> &nearest_neighbors, vector<double> input)
{
    vector<double>bin;
    if(nearest_neighbors.size()>1)
    {
        for(int row1 =0 ;row1<nearest_neighbors.size();row1++)
        {
            for(int row =row1+1 ;row<nearest_neighbors.size();row++)
            {
                if(LDistance(input,nearest_neighbors[row1])>LDistance(input,nearest_neighbors[row]))
                {
                    bin=nearest_neighbors[row1];
                    nearest_neighbors[row1]=nearest_neighbors[row];
                    nearest_neighbors[row]=bin;
                }
            }
        }
    }
    return nearest_neighbors;
}
/*
Searching subtree node to find the nearest neighbor
*/
void SearchSubtree(vector<vector<double>> &nearest_neighbors,vector<double> input,tree_node * sub_node,int k )
{
    if(sub_node==NULL){return;}
    // sorting the nearest node list
    nearest_neighbors=SortNeighbors(nearest_neighbors,input);
    // recursiving search the sub kd tree
    double largest_dis =LDistance(input,nearest_neighbors[nearest_neighbors.size()-1]);
    if(abs(sub_node->row_data[sub_node->split_dimention+1]-input[sub_node->split_dimention])<largest_dis)
    {
        if(nearest_neighbors.size()<k+1)
        {
            nearest_neighbors.push_back(sub_node->row_data);
        }
        else
        {
            nearest_neighbors[nearest_neighbors.size()-1]=sub_node->row_data;
        }
        SearchSubtree(nearest_neighbors,input,sub_node->left,k);
        SearchSubtree(nearest_neighbors,input,sub_node->right,k);
    }
    else
    {
        //searching left if mid value less than input
        if(sub_node->row_data[sub_node->split_dimention+1] > input[sub_node->split_dimention])
        {
            SearchSubtree(nearest_neighbors,input,sub_node->left,k);
        }
        else
        {
            SearchSubtree(nearest_neighbors,input,sub_node->right,k);
        }
        
    }
}

/*

this method is to find the nearest neighbor in the kd tree
*/
vector<vector<double>> SearchKDtree(int feature_num,vector<double> input,int k )
{
    tree_node* r = root;
    tree_node* leaf_node;
    vector<vector<double>> nearest_neighbors;
    vector<double>bin;
    vector<double> distance;
    int dim =0;
    //find the leaf node
    while(r!=NULL)
    {
        leaf_node =r;
        if(input[dim]<r->row_data[dim+1])
        { 
            r=r->left;
        }
        else
        {
            r=r->right;  
        }  
        dim=(dim+1)%feature_num;     
    }
    // include the leaf node
    nearest_neighbors.push_back(leaf_node->row_data);
    r=leaf_node;
    //search kd tree 
    while (r->parent!=NULL)
    {
        r=r->parent;// go to parent node

        // sorting the nearest node list
        nearest_neighbors=SortNeighbors(nearest_neighbors,input);
        // if the current node data less than the bigest nearest neighbor then go to the right node
        double largest_dis =LDistance(input,nearest_neighbors[nearest_neighbors.size()-1]);
        if(nearest_neighbors.size()<k+1)
        {       
            nearest_neighbors.push_back(r->row_data);
        }
        else if(nearest_neighbors.size()>=k+1 && LDistance(input,r->row_data)<largest_dis)
        {
            bin=r->row_data;
            nearest_neighbors[nearest_neighbors.size()-1]=bin;
        }     
     
    
        
        tree_node * son;
        // search another subtree
        if(abs(r->row_data[r->split_dimention+1]-input[r->split_dimention])<largest_dis)
        {
            if(r->row_data[r->split_dimention+1] > input[r->split_dimention])// searching right if mid value less than input
            {              
                son=r->right;
            }  
            else
            {
                son=r->left;;
            }    
             SearchSubtree(nearest_neighbors, input,son,k); 
             
        }
        
    }
    return nearest_neighbors;
}




int main()
{
    string train_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris.csv";
    string test_path="/home/fengchao/MachineLearning-CPP/my-code/statistics-learning-method-for-cpp/iris-data/Iris_test.csv";
    int tr_data_row=150;
    int te_data_row=15;
    int data_column=5;
    int k_value=12;
    ConstructKdTree(train_path,tr_data_row);
    printKdTreeNode(root);
    cout<<endl;
    vector<double> input;
    input.push_back(6);
    input.push_back(4);
    input.push_back(0.4);
    input.push_back(0.4);
    vector<vector<double>> nearest_neighbor;
    nearest_neighbor=SearchKDtree(4,input,5);

    //distance 
    vector<double>distance;
    for(int i=0;i<x_train_data.size();i++)
    {
        distance.push_back(LDistance(input,x_train_data[i]));
    }
    // sorting distance
    double bin;
    for(int row1 =0 ;row1<distance.size();row1++)
    {
        for(int row =row1+1 ;row<distance.size();row++)
        {
            if(distance[row1] > distance[row])
            {
                bin=distance[row1];
                distance[row1]=distance[row];
                distance[row]=bin;
            }
        }
    }

    //distance1 and distance2
    vector<double>distance1;
    for(int i =0 ; i<5;i++)
    {
        distance1.push_back(LDistance(input,nearest_neighbor[i]));
        cout<<"distance:    "<<distance[i]<<"distance1:   "<<LDistance(input,nearest_neighbor[i])<<endl;
    }

    cout<<"searching end"<< endl;
}




// /*
// antother method to search kd sub tree
// */
// void searchKdTreeNode(vector<vector<double>> &nearest_neighbors,vector<double> d,double &minDis,tree_node * &nearNode,tree_node * tmp,int k)
// {
//     //stop
//     if (tmp == NULL){return;}
//     // 判断当前节点是否小于
//     if (LDistance(d,tmp->row_data) < minDis)
//     {
//         nearest_neighbors.push_back(tmp->row_data);
//         minDis = LDistance(d,tmp->row_data);
//         nearNode = tmp;
//     }
//     // 如果轴与节点的距离小于minDis，则两个半边都需要搜索，否则只需要搜索一个半边
//     if (abs(tmp->row_data[tmp->split_dimention+1] - d[tmp->split_dimention]) < minDis)
//     {
//         searchKdTreeNode(nearest_neighbors, d, minDis, nearNode, tmp->left,k);
//         searchKdTreeNode(nearest_neighbors, d, minDis, nearNode, tmp->right,k);
//     }
//     else
//     {
//     // 选择搜索的一个半边
//         if (tmp->row_data[tmp->split_dimention+1] > d[tmp->split_dimention])
//             searchKdTreeNode(nearest_neighbors, d, minDis, nearNode, tmp->left,k);
//         else
//             searchKdTreeNode(nearest_neighbors, d, minDis, nearNode, tmp->right,k);
//     }
// }
// /*
// another method to travesal kd tree
// */
// vector<vector<double>> SearchKDtree1(int feature_num,vector<double> input,int k=1)
// {
//     tree_node* r = root;
//     tree_node* temp;
//     vector<vector<double>> nearest_neighbors;
//     vector<double>bin;
//     vector<double> distance;
//     int dim =0;
//     //find the leaf node
//     while(r!=NULL)
//     {
//         temp =r;
//         if(input[dim]<r->row_data[dim+1])
//         { 
//             r=r->left;
//         }
//         else
//         {
//             r=r->right;  
//         }  
//         dim=(dim+1)%feature_num;     
//     }
//     // include the leaf node
//     r=temp;// go to parent node
//     //bin=temp->row_data;
//     //nearest_neighbors.push_back(bin);
//     double minDis=LDistance(input, temp->row_data);
//     tree_node * nearNode;
//     // 退回到根节点
//     while (r->parent != NULL)
//     {
//         r = r->parent;       
//         // 判断父节点是否更近，如果近，更新最近节点
//         if (LDistance(input, r->row_data)< minDis)
//         {
            
//             nearest_neighbors.push_back(r->row_data);
//             nearNode = r; 
//             minDis = LDistance(input, r->row_data);
//         }
//         tree_node * son;
//         // 判断当前轴与点的距离，如果小于minDis，则进行到另一半进行查找
//         if (abs(r->row_data[r->split_dimention+1]-input[r->split_dimention]) < minDis)
//         {
//             //search another subtree which is not travesal in the first time
//             if(r->row_data[r->split_dimention+1]>input[r->split_dimention]) 
//                 son = r->right;
//             else
//                 son = r->left;
//             searchKdTreeNode(nearest_neighbors, input, minDis, nearNode, son,k);
//         }
//     }   
//     return nearest_neighbors;
// }
