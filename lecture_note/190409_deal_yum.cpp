

1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105
106
107
108
109
110
111
112
113
114
115
116
117
118
119
120
121
122
123
124
125
126
127
128
129
130
131
132
133
134
135
136
137
138
139
140
141
142
143
144
145
146
147
148
149
150
151
152
153
154
155
156
157
158
159
160
161
162
 
﻿#include "pch.h"
#include <iostream>
#include <cassert>
#include <vld.h>
 
#define SAFE_DELETE(x)      { if((x)!= nullptr) { delete (x); (x) = nullptr; } }
#define SAFE_DELETE_ARR(x)  { if((x)!= nullptr) { delete[] (x); (x) = nullptr; } }
 
#define EXPECT_EQ(x,y)      { assert((x)==(y) && "not same"); }
 
class Merchant
{
public:
 
    enum class eMerchantType
    {
        Seller,
        Buyer,
    };
 
public:
    Merchant(int a_nMoney, int a_nItemCount) :
        m_nMoney(a_nMoney), m_nItemCount(a_nItemCount)
    {
        // 
    }
 
    virtual void Info() const
    {
        std::cout << "돈 : " << m_nMoney << std::endl;
        std::cout << "개수 : " << m_nItemCount << std::endl;
    }
 
 
 
    int GetCount()
    {
        return m_nItemCount;
    }
 
    static const char* GetMerchatName(eMerchantType a_eType)
    {
        switch (a_eType)
        {
        case Merchant::eMerchantType::Seller:
            return "셀러";
            break;
        case Merchant::eMerchantType::Buyer:
            return "바이어";
            break;
        default:
            assert(false && "arg error");
            break;
        }
    }
 
    virtual eMerchantType GetType() = 0;
 
protected:
 
    int m_nMoney = 0;
    int m_nItemCount = 0;
};
 
class Seller : public Merchant
{
public:
 
    Seller(int a_nMoney, int a_nItemCount, int a_nItemPrice) :
        Merchant(a_nMoney, a_nItemCount),
        m_nItemPrice(a_nItemPrice)
    {
    }
 
    virtual void Info() const override
    {
        Merchant::Info();
        std::cout << "가격 : " << m_nItemPrice << std::endl;
    }
 
    int GetItemPrice()
    {
        return m_nItemPrice;
    }
 
    void Buy(int a_nCount)
    {
        assert(a_nCount > 0 && "arg error");
 
        m_nItemCount -= a_nCount;
        m_nMoney += (m_nItemPrice * a_nCount);
 
        // 잘 팜 
    }
 
    // Merchant을(를) 통해 상속됨
    virtual eMerchantType GetType() override
    {
        return eMerchantType::Seller;
    }
 
private:
 
    int m_nItemPrice;
};
 
class Buyer : public Merchant
{
public:
 
    Buyer(int a_nMoney, int a_nItemCount) : Merchant(a_nMoney, a_nItemCount) { }
 
    bool Buy(int a_nCount, Seller* a_pSeller)
    {
        if (a_nCount <= 0 || a_pSeller == nullptr) {
            //cout << "arg error" << ed
            return false;
        }
 
        int nCount = a_pSeller->GetCount();
 
        if (a_nCount > nCount) { /*cout << "셀러가 몇개없으" << ed*/return false; }
 
        int nPrice = a_pSeller->GetItemPrice();
        nPrice *= nCount;
 
        if (m_nMoney < nPrice) { return false; }
 
        a_pSeller->Buy(a_nCount);
 
        m_nItemCount += a_nCount;
        m_nMoney -= nPrice;
    }
 
    // Merchant을(를) 통해 상속됨
    virtual eMerchantType GetType() override
    {
        return eMerchantType::Buyer;
    }
};
 
 
 
int main(int n, char* s[])
{
    using namespace std;
 
    Merchant * p1 = new Buyer(1000, 10);
    Merchant * p2 = new Seller(1000, 1000, 10);
 
 
     
 
    cout << Merchant::GetMerchatName(p1->GetType()) << endl;
    cout << Merchant::GetMerchatName(p2->GetType()) << endl;
 
    SAFE_DELETE(p1);
    SAFE_DELETE(p2);
 
    getchar();
    return 0;
}
 
