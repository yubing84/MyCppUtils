#include <iostream>
#include "Singleton.h"

using namespace std;

class A
{
public:
	typedef std::shared_ptr<A> ptr;
	A()
	{
		std::cout << "¹¹Ôì" << std::endl;
		m_Value = 10;
	};

	virtual ~A()
	{

	};

	static A::ptr GetSingleton()
	{
		return Singleton<A>::GetInstance();
	}

public:
	int m_Value;
};

int main()
{
	A::ptr m_pA = Singleton<A>::GetInstance();

	std::cout << "m_Value = " << m_pA->m_Value << std::endl;

	std::cout << "m_Value2 = " << Singleton<A>::GetInstance()->m_Value << std::endl;

	std::cout << "m_Value3 = " << A::GetSingleton()->m_Value << std::endl;

	getchar();

	return 0;
}