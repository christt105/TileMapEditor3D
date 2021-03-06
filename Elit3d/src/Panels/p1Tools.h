#pragma once
#include "Panels/Base/Panel.h"

class p1Tools :
    public Panel
{
public:
    enum class Tools {
        NONE = -1,

        BRUSH,
        ERASER,
        BUCKET,
        EYEDROPPER,
        RECTANGLE,

        MAX
    };

    enum class Shape {
        RECTANGLE,
        CIRCLE
    };
    p1Tools(bool start_active, bool appear_mainmenubar, bool can_close);
    ~p1Tools();

    void Start() override;
    void Update() override;
    Tools GetSelectedTool() const;

    int GetToolSize() const;
    Shape GetToolShape() const;

    Tools selectedTool = Tools::BRUSH;

private:
    std::map<Tools, std::string> tools;

    int brushSize = 1;
    Shape shape = Shape::RECTANGLE;
};

