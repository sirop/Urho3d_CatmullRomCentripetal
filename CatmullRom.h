//#ifndef CATMULLROM_H
//#define CATMULLROM_H

#pragma once

#include "Sample.h"
#include "MySplinePath/SplinePath.h"

namespace Urho3D
{

class Drawable;
class Node;
class Scene;
}

class CatmullRom : public Sample
{
    URHO3D_OBJECT(CatmullRom, Sample);

public:
    /// Construct.
    CatmullRom(Context* context);

    /// Setup after engine initialization and before running the main loop.
    virtual void Start();



private:


    /// Construct the scene content.
    void CreateScene();
    /// Construct user interface elements.
    void CreateUI();
    /// Set up a viewport for displaying the scene.
    void SetupViewport();
   // void SetupViewports();
    /// Subscribe to application-wide logic update and post-render update events.
    void SubscribeToEvents();
    //void UnSubscribeFromEvents(StringHash eventType, VariantMap& eventData);

    /// Read input and moves the camera.
    void MoveCamera(float timeStep);
    /// Set path start or end point.
   // void SetPathPoint();


    /// Utility function to raycast to the cursor position. Return true if hit
    //bool Raycast(float maxDistance, Vector3& hitPos, Drawable*& hitDrawable);
    /// Make Jack follow the Detour path.
   // void FollowPath(float timeStep);
    /// Handle the logic update event.
    void HandleUpdate(StringHash, VariantMap& eventData);
    /// Handle the post-render update event.
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
    void HandleKeyUp(StringHash eventType, VariantMap& eventData);



   // void HandleStartButtonPressed(StringHash eventType, VariantMap& eventData);
    void HandleControlClicked(StringHash eventType, VariantMap& eventData);

    /// Last calculated path.
   // PODVector<Vector3> currentPath_;
    /// Path initial position.
    Vector3 initJackPos_;
    /// Path end position.
    Vector3 finalJackPos_;

    float jackPan_;

    /// Jack scene node.
    SharedPtr<Node> jackNode_;
    /// Flag for drawing debug geometry.
    bool drawDebug_;

    /// The Window.
   // SharedPtr<Window> window_;
     ResourceCache* cache_;
    /// The UI's root UIElement.
    SharedPtr<UIElement> uiRoot_;
    SharedPtr<Text> text_;
   // SharedPtr<SplineEditor> splineEditor;
    SplinePath* path_;
    Node* pathNode_;
    //Vector<WeakPtr<Node>> vecNodes;
//    Node* startNode;
//    Node* targetNode;

//    unsigned short gType_;
//    bool bStart_;
//    uint8_t returnX_=0;

    //float jackPan_;
    BoundingBox bounds_;
  //  Vector3 dollyDir_;
    //float dollyFactor_ = 1.0f;

    float cX, cY, cZ, cTilt, cPan, cXend, cYend, cZend ;

   // SharedPtr<Mover> mover; // = NULL;

    // Menu window
 //   SharedPtr<Window>  window_;
    // Slider The Window.
  //  SharedPtr<SlidersWindow> slidersWindow_;
    // Window 'titlebar' container
//    UIElement* titleBarSlider;
//    // the Window title Text
//    Text* windowTitleSlider;


   // bool jackFluct_ = false;

    String SliderString(String t, float number);

//     SharedPtr<Window> windowDolly_;
//     Text* windowDollyTitle;

     Text* instructionText;

//     /// Overview camera scene node.
//     SharedPtr<Node> overCameraNode_;
//     bool camera0;

};


//#endif // CATMULLROM_H
