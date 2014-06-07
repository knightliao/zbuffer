//Matrix.h
#ifndef MATRIX
#define MATRIX

//#define debug_mode

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

#define MATRIX_MIN 1e-10														//设定运算精度

class Matrix
{
public:
	//专用向量模板类：vector保护型派生类，目的是限制功能调用和提供安全下标访问
	template <class T>
	class m_vector;
	
	//构造函数:构造空矩阵
	Matrix(int row=0,int col=0,double value=0.0);

	//构造函数:使用已有数组构造矩阵
	Matrix(const double* first,const double* last,int row,int col=1);

	//构造函数:使用迭代器标记的数据区间构造矩阵
	template <class Iter>//(Iter需定义*Iter和++Iter运算)						//太好了，类里边还可以写模板函数
	Matrix(const Iter first,const Iter last,int row,int col=1);

	//重新设置矩阵行数
	void SetRow(int row,double value=0.0);

	//重新设置矩阵列数
	void SetCol(int col,double value=0.0);
	
	//判断矩阵相等 返回结果
	bool operator == (const Matrix& s) const;

	//判断矩阵不等 返回结果
	bool operator != (const Matrix& s) const{return !(*this==s);}
	
	//矩阵相加 返回和矩阵
	Matrix operator + (const Matrix& s) const;

	//矩阵相加 返回矩阵引用
	Matrix& operator += (const Matrix& s);

	//矩阵相减 返回差矩阵
	Matrix operator - (const Matrix& s) const;

	//矩阵相减 返回矩阵引用
	Matrix& operator -= (const Matrix& s);

	//矩阵与常数相乘 返回乘积矩阵
	Matrix operator * (double time) const;

	//矩阵与常数相乘 返回矩阵引用
	Matrix& operator *= (double time);
	
	//矩阵与常数相乘 返回乘积矩阵
	friend Matrix operator * (double time,const Matrix& s){return s*time;}

	//矩阵与矩阵相乘 返回乘积矩阵
	Matrix operator * (const Matrix& s) const;

	//矩阵与矩阵相乘 返回矩阵引用
	Matrix& operator *= (const Matrix& s);

	//求矩阵转置 返回原矩阵的转置矩阵
	Matrix Transpose() const; 

	//求逆矩阵 返回方阵的逆矩阵
	Matrix Converse() const;
	
	//伴随矩阵 返回方阵的伴随矩阵
	Matrix Adjoint() const;
	
	//求指定a(i,j)代数余子式 返回double型结果
	double Cofactor(int i,int j) const;//(i,j从1开始)
	
	//求行列式 返回方阵的行列式值 
	double Determinate() const;
	
	//求秩 返回矩阵的秩
	int Rank() const;
	
	//单位矩阵 返回方阵对应的单位矩阵
	Matrix Unit() const;
	
	//初等变换：用time乘以矩阵某一行（列）(time!=0) 返回变换后的矩阵本身
	Matrix& Time(int i,double time=1.0,bool isRow=true);//(i从1开始)
	
	//初等变换：交换两行（列） 返回变换后的矩阵本身
	Matrix& Exchange(int i,int j,bool isRow=true);//(i,j从1开始)
	
	//初等变换：将第i行（列）的time倍加到第j行（列）（i!=j） 返回变换后的矩阵本身
	Matrix& Add(int i,int j,double time=1.0,bool isRow=true);//(i,j从1开始)

	//求两矩阵形成的增广矩阵 返回该增广矩阵
	Matrix operator | (const Matrix& B) const;

	//使用从1开始的行号和列号访问矩阵元素 返回对应元素引用
	double& operator () (int row,int col=1);

	//使用从1开始的行号和列号访问矩阵元素 返回对应元素常引用
	const double& operator () (int row,int col=1) const;

	//使用从0开始的行号和列号访问矩阵元素 返回对应元素的引用
	Matrix::m_vector<double>& operator [] (int rowIndex);//哥们儿技止此耳,a[i][j]中j越界是检查不了了.
	/*solved:用类继承（保护继承）将底层vector封起来，
	公有接口只提供下标访问（安全的重载函数），
	保护接口让友元类访问原vector接口不受影响*/

	//使用从0开始的行号和列号访问矩阵元素 返回对应元素的常引用
	const Matrix::m_vector<double>& operator [] (int rowIndex) const;

	//高斯消元法解方程 返回方程解(列向量)(最后一列为特解，其他列为齐次解，空矩阵表示无解)
	friend Matrix Gause(const Matrix& A,const Matrix& B);

	//求矩阵行数 返回矩阵行数
	int Row() const {return m_row;}

	//求矩阵列数 返回矩阵列数
	int Col() const {return m_col;}
	
#ifdef debug_mode
	//将矩阵打印函数声明为友元函数，便于调试时查看状态
	friend ostream& operator << (ostream& out,const Matrix& s);
#endif //debug_mode
protected:
    vector<int> m_gause();//保护成员函数:初等行变换至行首系数为1的阶梯矩阵
	vector<m_vector<double> > m_c;//保护成员:矩阵数据存储区
	int m_row;//保护成员:行数
	int m_col;//保护成员:列数
};

template <class T>
class Matrix::m_vector:protected vector<T>
{//套这一个类，只为下标安全访问和对外向量接口禁用
//tbc:但是构造函数和拷贝构造函数仍无法对外禁用
public:
	m_vector(int dim=0,const T& value=T()):vector<T>(dim,value){}
	T& operator [] (int index)
	{
		if (index>=(int)vector<T>::size())//Pay attention!：int和unsigned int相比较时，会把int强制转换成unsigned int
		    throw overflow_error("m_vector::operator [] ():overflow error");
		else if (index<0)
			throw underflow_error("m_vector::operator [] ():underflow error");
		return vector<T>::operator [] (index);
	}
	const T& operator [] (int index) const /*{return (const T&)(*this)[index];}//不行*/
	{
		if (index>=(int)vector<T>::size())
		    throw overflow_error("m_vector::operator [] ():overflow error");
		else if (index<0)
			throw underflow_error("m_vector::operator [] ():underflow error");
		return vector<T>::operator [] (index);
	}
	friend class Matrix;
};

template <class Iter>
Matrix::Matrix(const Iter first,const Iter last,int row,int col)
{
	if (row<1 || col<1)
		throw exception("Matrix::Matrix(Iter,Iter,int,int):illegal input");

	//初始化
	m_row = row;												//行列初始化
	m_col = col;
	m_c.resize(row);											//空间初始化
	for (int i=0; i<row; i++)
		m_c[i].resize(col,0.0);
		
	//为矩阵赋值
	Iter iter=first;
	for (int i=0; i<row; i++)
	{
		for (int j=0; j<col; j++)
		{
			if (iter!=last)
			{
   			    if (fabs((double)(*iter))>=MATRIX_MIN)					//Pay attention!
					m_c[i][j] = *iter;
			    else
			    	m_c[i][j] = 0.0;
				++iter;
			}
			else
				throw exception("Matrix::Matrix(Iter,Iter,int,int):arguements not match");
		}
	}
	if (iter!=last)
		throw exception("Matrix::Matrix(Iter,Iter,int,int):arguements not match");
}

#endif //MATRIX
