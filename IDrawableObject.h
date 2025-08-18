#ifndef IDRAWABLEOBJECT_H
#define IDRAWABLEOBJECT_H

#include <vector>
#include <Windows.h>

class IDrawableObject {
public:
    virtual ~IDrawableObject() = default;

    // ���������� �������� ������� (��������� ������� ������)
    virtual const std::vector<std::vector<SHORT>>& GetTexture() const = 0;

    // ���������� ���������� ��������
    virtual COORD GetPosition() const = 0;
};

#endif
