#ifndef IPLAYERCONTROLLER_H
#define IPLAYERCONTROLLER_H

class IPlayerController {
public:
    virtual ~IPlayerController() = default;

    // ����� ��� ��������� ����� ������
    virtual void HandleInput(float deltaTime) = 0;

    // ����� ��� ���������� ������������ ������
    virtual void Update(float deltaTime) = 0;
};

#endif
