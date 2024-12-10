#ifndef IUPDATABLE_H
#define IUPDATABLE_H

class IUpdatable {
public:
    virtual ~IUpdatable() = default;

    // ����� ��� ���������� �������, ���������� ������ ����
    virtual void Update(float deltaTime) = 0;

    // ����� ��� ������������� ����������, ���������� � ���������� ���������� �������
    virtual void FixedUpdate(float fixedDeltaTime) = 0;
};

#endif
