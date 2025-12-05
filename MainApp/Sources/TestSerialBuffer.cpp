#include "pch.h"
#include "SerialBuffer.h"

int test_serial_buffer() 
{
    SerialBuffer buf;

    int a = 123;
    float b = 3.14f;
    short c = 456;

    // operator<< 로 쓰기
    buf << a;
    buf << b; 
    buf << c; 

    std::cout << "Used Size after operator<<: " << buf.GetUsedSize() << std::endl; // 10 (4+4+2)

    int a2 = 0;
    float b2 = 0.0f;
    short c2 = 0;

    // operator>> 로 읽기
    buf >> a2;
    buf >> b2; 
    buf >> c2;

    std::cout << "Read Values: " << a2 << ", " << b2 << ", " << c2 << std::endl;

    buf.Clear();

    // PutData 체인 호출 테스트
    buf.PutData(a).PutData(b).PutData(c);

    std::cout << "Used Size after PutData: " << buf.GetUsedSize() << std::endl; // 10 (4+4+2)

    // GetData 테스트
    a2 = 0; b2 = 0.0f; c2 = 0;
    buf.GetData(a2).GetData(b2).GetData(c2);

    std::cout << "Read Values after GetData: " << a2 << ", " << b2 << ", " << c2 << std::endl;

	return 0; // Success
}

// Disassembly 

/*
클래스 템플릿 특수화 + 헬퍼 함수 분리 패턴을 사용하여
SerialBuffer 클래스의 직렬화 및 역직렬화 기능을 구현했습니다.

극한 최적화 결과 증명 

buf >> a2 >> b2 >> c2;
00007FF6AD0F108A F3 0F 11 74 24 40    movss       dword ptr [rsp+40h],xmm6

 buf.GetData(a2).GetData(b2).GetData(c2);
00007FF6AD0F1122 8B 44 24 48          mov         eax,dword ptr [b]

    35:     buf.GetData(a2).GetData(b2).GetData(c2);
00007FF6AD0F1134 89 44 24 40          mov         dword ptr [b2],eax
*/