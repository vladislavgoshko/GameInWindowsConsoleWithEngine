#pragma once

class IUpdatable {
public:
    virtual ~IUpdatable() = default;

    // ����� ��� ���������� �������, ���������� ������ ����
    virtual void Update(float deltaTime) = 0;

    // ����� ��� ������������� ����������, ���������� � ���������� ���������� �������
    virtual void FixedUpdate(float fixedDeltaTime) = 0;
};