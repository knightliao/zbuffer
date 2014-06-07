//Matrix.cpp
#include "Matrix.h"

//成员函数实现
Matrix::Matrix(int row,int col,double value)
{
	if (row<0 || col<0)
		throw exception("Matrix::Matrix(int,int):illegal input");
	m_row = row;												//没说的，矩阵初始化，各元素置零
	m_col = col;
	m_c.resize(row);
	for (int i=0; i<row; i++)
	{
		m_c[i].resize(col,value);
	}
}

Matrix::Matrix(const double* first,const double* last,int row,int col)
{
	if (row<1 || col<1)
		throw exception("Matrix::Matrix(double*,double*,int,int):illegal input");
	if (row*col!=(last-first))
		throw exception("Matrix::Matrix(double*,double*,int,int):arguements not match");

	const double *p=first;										//工作指针初始化
	m_row = row;												//行列初始化
	m_col = col;
	m_c.resize(row);											//向量大小初始化
	for (int i=0; i<row; i++)									//矩阵各元素初始化
	{
		m_c[i].resize(col);
		for (int j=0; j<col; j++)
		{
		 	if (fabs(*p)>=MATRIX_MIN)
				m_c[i][j] = *p++;
		    else
		    	m_c[i][j] = 0.0;
		}
	}
}

void Matrix::SetRow(int row,double value)
{
	if (row<0)
		throw exception("Matrix::SetRow():illegal input");
	m_row = row;
	m_c.resize(row,m_vector<double>(m_col,value));
}


void Matrix::SetCol(int col,double value)
{
	if (col<0)
		throw exception("Matrix::SetCol():illegal input");
	m_col = col;
	for (int i=0; i<m_row; i++)
		m_c[i].resize(col,value);
}

bool Matrix::operator == (const Matrix& s) const
{
	if (m_row!=s.m_row || m_col!=s.m_col) return false;		//行列数不等，矩阵不等
	for (int i=0; i<m_row; i++)
	    for (int j=0; j<m_col; j++)
	        if (fabs(m_c[i][j]-s.m_c[i][j])>=MATRIX_MIN)
				return false;								//有元素值不对应相等，矩阵不等
	return true;
}

Matrix Matrix::operator + (const Matrix& s) const
{
	if (m_row!=s.m_row || m_col!=s.m_col)
		throw exception("Matrix::operator + ():illegal input");
	Matrix sum(m_row,m_col);
	for (int i=0; i<m_row; i++)
		for (int j=0; j<m_col; j++)
		{
			sum.m_c[i][j] = m_c[i][j] + s.m_c[i][j];
			if (fabs(sum.m_c[i][j])<MATRIX_MIN)
			   sum.m_c[i][j] = 0.0;
		}
	return sum;
}

Matrix& Matrix::operator += (const Matrix& s)
{
	if (m_row!=s.m_row || m_col!=s.m_col)
		throw exception("Matrix::operator += ():illegal input");
	for (int i=0; i<m_row; i++)
		for (int j=0; j<m_col; j++)
		{
			m_c[i][j] += s.m_c[i][j];
			if (fabs(m_c[i][j])<MATRIX_MIN)
			    m_c[i][j] = 0.0;
		}
	return *this;
}

Matrix Matrix::operator - (const Matrix& s) const
{
	if (m_row!=s.m_row || m_col!=s.m_col)
		throw exception("Matrix::operator - ():illegal input");
	Matrix dif(m_row,m_col);
	for (int i=0; i<m_row; i++)
		for (int j=0; j<m_col; j++)
		{
			dif.m_c[i][j] = m_c[i][j] - s.m_c[i][j];
			if (fabs(dif.m_c[i][j])<MATRIX_MIN)
			   dif.m_c[i][j] = 0.0;
		}
	return dif;
}

Matrix& Matrix::operator -= (const Matrix& s)
{
	if (m_row!=s.m_row || m_col!=s.m_col)
		throw exception("Matrix::operator -= ():illegal input");
	for (int i=0; i<m_row; i++)
		for (int j=0; j<m_col; j++)
		{
			m_c[i][j] -= s.m_c[i][j];
			if (fabs(m_c[i][j])<MATRIX_MIN)
			   m_c[i][j] = 0.0;
		}
	return *this;
}

Matrix Matrix::operator * (double time) const
{
	Matrix mul=*this;
	for (int i=0; i<m_row; i++)
		for (int j=0; j<m_col; j++)
		{
			mul.m_c[i][j] = m_c[i][j]*time;
			if (fabs(mul.m_c[i][j])<MATRIX_MIN)
			   mul.m_c[i][j] = 0.0;
		}
	return mul;
}

Matrix& Matrix::operator *= (double time)
{
	for (int i=0; i<m_row; i++)
		for (int j=0; j<m_col; j++)
		{
			m_c[i][j] *= time;
			if (fabs(m_c[i][j])<MATRIX_MIN)
			   m_c[i][j] = 0.0;
		}
	return *this;
}

Matrix Matrix::operator * (const Matrix& s) const
{
	if (m_col!=s.m_row)
		throw exception("Matrix::operator * (Matrix&):illegal input");
	Matrix mul(m_row,s.m_col);
	for (int i=0; i<m_row; i++)
	{
		for (int j=0; j<s.m_col; j++)
		{
			mul.m_c[i][j] = 0.0;
			for (int k=0; k<m_col; k++)
				mul.m_c[i][j] += m_c[i][k]*s.m_c[k][j];
			if (fabs(mul.m_c[i][j])<MATRIX_MIN)
			    mul.m_c[i][j] = 0.0;
		}
	}
	return mul;
}

Matrix& Matrix::operator *= (const Matrix& s)
{
	if (m_col!=s.m_row)
		throw exception("Matrix::operator *= (Matrix&):illegal input");
	Matrix mul(m_row,s.m_col,0.0);
	for (int i=0; i<m_row; i++)
	{
		for (int j=0; j<s.m_col; j++)
		{
			for (int k=0; k<m_col; k++)
				mul.m_c[i][j] += m_c[i][k]*s.m_c[k][j];
			if (fabs(mul.m_c[i][j])<MATRIX_MIN)
			    mul.m_c[i][j] = 0.0;
		}
	}
	return *this=mul;
}

Matrix Matrix::operator | (const Matrix& B) const
{
	//if (m_row!=m_col || m_row!=B.m_row || B.m_col!=1)
	if (m_row!=B.m_row)
		throw exception("Matrix::operator | ():illegal input");
	Matrix extend(*this);
	for (int i=0; i<m_row; i++)
	{
		extend.m_c[i].resize(extend.m_col+B.m_col);
		for (int j=0; j<B.m_col; j++)
			extend.m_c[i][extend.m_col+j] = B.m_c[i][j];
	}
	extend.m_col += B.m_col;
	return extend;
}

Matrix Matrix::Transpose() const
{
	Matrix trans(m_col,m_row);
	for (int i=0; i<m_row; i++)
		for (int j=0; j<m_col; j++)
			trans.m_c[j][i] = m_c[i][j];
	return trans;
}

Matrix Matrix::Converse() const
{
	//检查矩阵，抛掷异常
	if (m_row<=0 || m_col<=0)
		throw exception("Matrix::Converse():矩阵参数非法");
	if (m_row != m_col)
		throw exception("Matrix::Converse():矩阵参数非法");
	int n=m_row;

	//用单位矩阵生成增广矩阵
	Matrix work(*this|this->Unit());

	// 高斯消元
	vector<int> msg=work.m_gause();
	
	//检查是否可逆
	if (msg.size()>0)
		throw exception("Matrix::Converse():矩阵不可逆");

	//返回结果
	Matrix Con(n,n);
	for (int i=0; i<n; i++)
		for (int j=0; j<n; j++)
			Con[i][j] = work[i][n+j];
	return Con;
}

Matrix Matrix::Adjoint() const
{
	if (m_row!=m_col)
		throw "Matrix::Adjoint():非方阵";
	double det=Determinate();
	if (fabs(det)>=MATRIX_MIN)
	{
		return det*Converse();
	}
	else
	{
		int n=m_row;
		Matrix adj(n,n);
		for (int i=0; i<n; i++)
			for (int j=0; j<n; j++)
				adj[i][j] = Cofactor(j+1,i+1);
		return adj;
	}
}

double Matrix::Cofactor(int i,int j) const
{
	i--,j--;                        	//适应数组习惯
	if (m_row!=m_col || m_row<2)
	    throw exception("Matrix::Cofactor():矩阵不满足该运算所需条件");
	if (i<0 || i>=m_row || j<0 || j>=m_col)
	    throw exception("Matrix::Cofactor():illegal input");
	Matrix work(m_row-1,m_col-1);
	for (int row=0,r=0; row<m_row; row++)
	{
		if (row==i)//Pay attention!
			continue;
		for (int col=0,c=0; col<m_col; col++)
		{
			if (col==j)
			    continue;
			work.m_c[r][c] = m_c[row][col];
			c++;
		}
		r++;
	}
	int sign=1;
	if ((i+j)&1) sign*=-1;          	//（i+j）为奇数时,代数余子式符号为-1
	/*cout<<"*this:"<<endl<<*this;
	cout<<"work with i="<<i+1<<";j="<<j+1<<";sign="<<sign<<endl<<work;
	system("pause");*/
	return sign*work.Determinate();
}

double Matrix::Determinate() const
{
 	if (m_col!=m_row)
	    throw exception("Matrix::Determinate():非方阵");
	if (m_row<1)
	    throw exception("Matrix::Determinate():矩阵不满足该运算所需条件");
    //通过转换为上三角矩阵求行列式的值
	Matrix work(*this);
	int n=m_row,sign=1;
	for (int i=0; i<n; i++)                                 //初等行变换为上三角矩阵
	{
		double maxi=fabs(work.m_c[i][i]);
		int maxIndex = i,j;
		for (j=i+1; j<n; j++)
		{
			if (maxi<fabs(work.m_c[j][i]))
			{
				maxi = fabs(work.m_c[j][i]);
				maxIndex = j;
			}
		}
		if (maxi<MATRIX_MIN) return 0.0;					//秩小于n，行列式为零
		if (maxIndex != i)
		{
			m_vector<double> tmp=work.m_c[i];
			work.m_c[i] = work.m_c[maxIndex];
			work.m_c[maxIndex] = tmp;
			sign *= -1;                                     //对换改变行列式符号
		}
		for (j=i+1; j<n; j++)
		{
			double time=work.m_c[j][i]/work.m_c[i][i];
			for (int k=0; k<n; k++)
			{
			 	work.m_c[j][k] -= time*work.m_c[i][k];
				if (fabs(work.m_c[j][k])<MATRIX_MIN)
					work.m_c[j][k] = 0.0;
			}
 		}
	}

	//返回结果
	double det=sign;
	for (int i=0; i<n; i++)
		det *= work.m_c[i][i];
	return det;
}

int Matrix::Rank() const
{
	//用初等行变换变换至阶梯型，非零行数即秩
	Matrix work(*this);
	vector<int> msg=work.m_gause();
	return m_col-(int)msg.size();
}

Matrix Matrix::Unit() const
{
 	if (m_col!=m_row)
	    throw exception("Matrix::Unit():非方阵");
    Matrix unit(m_row,m_row,0.0);
    for (int i=0; i<m_row; i++)
    	unit[i][i] = 1.0;
   	return unit;
}

Matrix& Matrix::Time(int i,double time,bool isRow)
{
	i--;                        		//适应数组习惯
	if (fabs(time)<MATRIX_MIN)
	    throw exception("Matrix::Time():illegal input：倍数不能为零");
	if (isRow)							//行变换
	{
		if (i<0 || i>=m_row)
		    throw exception("Matrix::Time():illegal input：行号溢出");
		for (int t=0; t<m_col; t++)
		{
		    m_c[i][t] *= time;
		    if (fabs(m_c[i][t])<MATRIX_MIN)
		        m_c[i][t] = 0.0;
		}
	}
	else								//列变换
	{
		if (i<0 || i>=m_col)
		    throw exception("Matrix::Time():illegal input：列号溢出");
		for (int t=0; t<m_col; t++)
		{
		    m_c[t][i] *= time;
		    if (fabs(m_c[t][i])<MATRIX_MIN)
		        m_c[t][i] = 0.0;
		}
	}
	return *this;
}

Matrix& Matrix::Exchange(int i,int j,bool isRow)
{
	i--,j--;                        	//适应数组习惯
	if (i<0 || j<0 )
	    throw exception("Matrix::Exchange():illegal input");
	if (isRow)							//行变换
	{
		if (i>=m_row || j>=m_row)
		    throw exception("Matrix::Exchange():illegal input");
		if (i==j)   return *this;
		double tmp;
		for (int t=0; t<m_col; t++)
		{
			tmp = m_c[i][t];
			m_c[i][t] = m_c[j][t];
			m_c[j][t] = tmp;
		}
	}
	else								//列变换
	{
		if (i>=m_col || j>=m_col)
		    throw exception("Matrix::Exchange():illegal input");
		if (i==j)   return *this;
		double tmp;
		for (int t=0; t<m_row; t++)
		{
			tmp = m_c[t][i];
			m_c[t][i] = m_c[t][j];
			m_c[t][j] = tmp;
		}
	}
	return *this;
}

Matrix& Matrix::Add(int i,int j,double time,bool isRow)
{
	i--,j--;                        	//适应数组习惯
	if (i<0 || j<0 || i==j)
	    throw exception("Matrix::Add():illegal input");
	if (isRow)							//行变换
	{
		if (i>=m_row || j>=m_row)       //保证行索引合法
		    throw exception("Matrix::Add():illegal input");
		for (int t=0; t<m_col; t++)
		{
		    m_c[j][t] += m_c[i][t]*time;
		    if (fabs(m_c[j][t])<MATRIX_MIN)
		        m_c[j][t] = 0.0;
		}
	}
	else								//列变换
	{
		if (i>=m_col || j>=m_col)       //保证列索引合法
		    throw exception("Matrix::Add():illegal input");
		for (int t=0; t<m_row; t++)
		{
			m_c[t][j] += time*m_c[t][i];
			if (fabs(m_c[t][j])<MATRIX_MIN)
				m_c[t][j] = 0.0;
		}
	}
	return *this;
}

double& Matrix::operator () (int row,int col)
{
	if (row<1 || row>m_row || col<1 || col>m_col)
		throw exception("Matrix::operator () :访问溢出");
	return m_c[row-1][col-1];
}

const double& Matrix::operator () (int row,int col) const
{
	if (row<1 || row>m_row || col<1 || col>m_col)
		throw exception("Matrix::operator () :访问溢出");
	return m_c[row-1][col-1];
}

Matrix::m_vector<double>& Matrix::operator [] (int rowIndex)
{
	if (rowIndex>=m_row)
	    throw overflow_error("Matrix::operator [] ():overflow error");
	else if (rowIndex<0)
		throw underflow_error("Matrix::operator [] ():underflow error");
	return m_c[rowIndex];
}

const Matrix::m_vector<double>& Matrix::operator [] (int rowIndex) const
{
	if (rowIndex>=m_row)
	    throw overflow_error("Matrix::operator [] ():overflow error");
	else if (rowIndex<0)
		throw underflow_error("Matrix::operator [] ():underflow error");
	return m_c[rowIndex];
}

Matrix Gause(const Matrix& A,const Matrix& B)
{
	//if (A.m_row!=A.m_col || A.m_row!=B.m_row || B.m_col!=1)
	if (A.m_row!=B.m_row || A.m_col<1 || B.m_col!=1)//方程平衡 至少有一个未知数 等号右边为列向量
		throw exception("Gause():illegal input");

	//生成增广矩阵
	Matrix work(A|B);

	// 高斯消元
	vector<int> info=work.m_gause();

	//向主调函数返回运算结果
	Matrix sln(A.m_col);			//未知数个数作为解向量维数
	int row=work.m_row,col=work.m_col,d=info.size(),r,i;
	//检验方程是否有解
	for (r=row-1; r>=0; r--)        //在增广矩阵最后一列中找最后一个不为零的数work[r][col-1]
	    if (fabs(work[r][col-1])>=MATRIX_MIN)
	        break;
	if (r>=0)
	{
		for (i=col-2; i>=0 && fabs(work[r][i])<MATRIX_MIN; i--);//在第r行前n-1列找不为零的数
		if (i<0) return sln;		//若找不到，则方程无解
	}
	//方程有解
	if (d>0 && info[d-1]==col-1) d--;//消除增广矩阵末尾列影响
	sln.SetCol(d+1);
	for (i=0; i<d; i++)     		//齐次解
	{
		int delta=0;				//从work[][info[i]]到sln[][i]的映射
  		for (int j=0; j<info[i]; j++)
		{
			if (j==info[delta])
			{
				sln[j][i] = 0;
				delta++;
				continue;
			}
			sln[j][i] = -1*work[j-delta][info[i]];
		}
		sln[info[i]][i] = 1;
	}
	//特解//Pay attention!
	if (d>0)
	{
		int delta=0;
		for (i=0; i<sln.m_row && i-delta<work.m_row; i++)//从work[][col-1]到sln[][d]的映射
		{
			if (i==info[delta])//info[delta]随delta增加速度大于或等于1
			{
				sln[i][d] = 0;
				delta++;
				continue;
			}
			sln[i][d] = work[i-delta][col-1];
		}
	}
	else
		for (i=0; i<sln.m_row && i<work.m_row; i++)
		    sln[i][d] = work[i][col-1];
	while (i<sln.m_row) sln[i++][d] = 0;
	return sln;
}

vector<int> Matrix::m_gause()
{
	vector<int> zero;       							//返回值向量

	// 高斯消元(初等行变换至行首系数为1的阶梯矩阵)
	/*
	   1,0,...,0,a,0,...,0,b,o,0,...,b1
	   0,1,...,0,c,0,...,0,d,p,0,...,b2
	   ................................
	   0,0,...,1,e,0,...,0,g,q,0,...,b(i-1)
	   0,0,...,0,0,1,...,0,h,s,0,...,b(i+1)
	   ................................
	   0,0,...,0,0,0,...,1,m,t,0,...,b(j-1)
	   0,0,...,0,0,0,...,0,0,0,1,...,b(j+2)
	   ................................
	   0,0,...,0,0,0,...,0,0,0,0,...,br
	*/
	for (int c=0,workRow=0; c<m_col && workRow<m_row; c++)		//变换进行轮数(当前工作行)
	{
		double max=fabs(m_c[workRow][c]);
		int maxRow=workRow,i;
		for (i=workRow+1; i<m_row; i++)					//寻找当前列中当前工作行及以下最大值
		{
			if (abs(m_c[i][c])>max)
			{
				max = fabs(m_c[i][c]);
				maxRow = i;
			}
		}
		/*if (max<MATRIX_MIN)                    		//抛掷异常处理秩小情况
			//throw exception("Matrix::m_gause():矩阵秩小于行数");*/
		if (max<MATRIX_MIN)                           	//简单跳过处理秩小情况
		{
			zero.push_back(c);
		   	continue;
		}
		if (maxRow!=workRow)							//换行使最大系数行在当前最前
		{
			m_vector<double> tmp(m_c[workRow]);
			m_c[workRow] = m_c[maxRow];
			m_c[maxRow] = tmp;
		}
		max = m_c[workRow][c];
		for (i=0; i<m_col; i++)							//当前工作行首系数置一
			m_c[workRow][i] /= max;
		for (i=0; i<m_row; i++)							//其他行首系数置零
		{
			if (i==workRow)
				continue;
			double time=m_c[i][c];
			for (int j=0; j<m_col; j++)
			{
				m_c[i][j] -= time*m_c[workRow][j];
				if (fabs(m_c[i][j])<MATRIX_MIN)			//精度控制
				    m_c[i][j] = 0.0;
			}
		}
		workRow++;                                      //当前工作行下移
	}
	return zero;
}
