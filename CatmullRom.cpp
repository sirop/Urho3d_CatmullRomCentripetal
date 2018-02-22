
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Engine/Engine.h>

#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationState.h>


#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Graphics/StaticModelGroup.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/RenderPath.h>  // for multiple viewports
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>


#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>

#include <Urho3D/DebugNew.h>

#include "CatmullRom.h"

#include <string>
#include <sstream>
#include <iomanip>


URHO3D_DEFINE_APPLICATION_MAIN(CatmullRom)


CatmullRom::CatmullRom(Context* context) :
    Sample(context),
    uiRoot_(GetSubsystem<UI>()->GetRoot()),
    drawDebug_(false)
{
    // Register an object factory for our custom Mover component so that we can create them to scene nodes
    //context->RegisterFactory<Mover>();

}

void CatmullRom::Start()
{
    // Execute base class startup
    Sample::Start();

    // Enable OS cursor
       GetSubsystem<Input>()->SetMouseVisible(true);
       //GetSubsystem<UI>()->SetCursorVisible(true);

       // Load XML file containing default UI style sheet
       cache_ = GetSubsystem<ResourceCache>();
       cache_->AddResourceDir(GetSubsystem<FileSystem>()->GetProgramDir());
       XMLFile* style = cache_->GetResource<XMLFile>("UI/DefaultStyle.xml");

       // Set the loaded style as default style
       uiRoot_->SetDefaultStyle(style);

       // Create the scene content
       CreateScene();

       // Create the UI content
       CreateUI();

       // Setup the viewport for displaying the scene
       SetupViewport();



    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_FREE);
}

void CatmullRom::CreateScene()
{
    //ResourceCache* cache = GetSubsystem<ResourceCache>();

    //if (scene_->Refs()==0)
    scene_ = new Scene(context_);

    // Create octree, use default volume (-1000, -1000, -1000) to (1000, 1000, 1000)
    // Also create a DebugRenderer component so that we can draw debug geometry
    scene_->CreateComponent<Octree>();
    debug_ = scene_->CreateComponent<DebugRenderer>();

    // Create scene node & StaticModel component for showing a static plane
    Node* planeNode = scene_->CreateChild("Plane");
    planeNode->SetScale(Vector3(200.0f,0.0f, 200.0f));
    StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache_->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache_->GetResource<Material>("Materials/Grids/GridBlue+.xml"));
//   Urho3D::SharedPtr<Urho3D::Texture2D> texture (new Urho3D::Texture2D(context_));
//   texture->SetSize(width(),height(),Urho3D::Graphics::GetRGBAFormat(),Urho3D::TEXTURE_STATIC);
//   texture->SetData(0,0,0,512,512,cache->GetResource<Image>("Textures/Grids/gridBlue512.png"));    Urho3D::SharedPtr<Urho3D::Material> material(new Urho3D::Material(context_));
//    material->SetTexture(TU_DIFFUSE, texture);
//    planeObject->SetMaterial(0,material);




    // Create a Zone component for ambient lighting & fog control
    Node* zoneNode = scene_->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
    zone->SetFogColor(Color(0.5f, 0.5f, 0.7f));
 // zone->SetFogStart(100.0f);
 // zone->SetFogEnd(300.0f);

    // Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
    // Set cascade splits at 10, 50 and 200 world units, fade shadows out at 80% of maximum shadow distance
    light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));



    // Create Jack node that will follow the path
    jackNode_ = scene_->CreateChild("Jack");
    initJackPos_ = Vector3(-50.0f, 0.0f,-50.0f);
    finalJackPos_ = Vector3(90.0f, 0.0f, -50.0f);
    jackNode_->SetPosition(initJackPos_);
    jackNode_->SetRotation(Quaternion(0.0f, jackPan_, 0.0f));
    jackNode_->SetScale(9.0f);






   // bounds_ = BoundingBox (Vector3(-100.0f, 0.0f, -100.0f), Vector3(100.0f, 0.0f, 100.0f));
   // mover = jackNode_->CreateComponent<Mover>();
   // mover->SetParameters(0.0f, 0.0f, bounds_, gType_, 0.0f, 0.0f);





    // Create the camera. Limit far clip distance to match the fog
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->SetName("WheelsCamera");
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFarClip(2000.0f);
    camera->SetNearClip(1.0f);
    camera->SetOrthographic(false);

    // Set an initial position for the camera scene node above the plane and looking down
    cameraNode_->SetPosition(Vector3(0.0f, 20.0f, -230.0f));
    cX=cXend = 0.0f;
    cY=cYend= 20.0f;
    cZ =cZend= -230.0f;
    cTilt = pitch_ = 5.0f;
    cPan = yaw_ = 0.0f;
    cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

    AnimatedModel* modelObject = jackNode_->CreateComponent<AnimatedModel>();
    modelObject->SetModel(cache_->GetResource<Model>("Models/Jack.mdl"));
    modelObject->SetMaterial(cache_->GetResource<Material>("Materials/Jack.xml"));
    modelObject->SetCastShadows(true);

    bounds_ = BoundingBox (Vector3(-100.0f, 0.0f, -100.0f), Vector3(100.0f, 0.0f, 100.0f));
   // mover = jackNode_->CreateComponent<Mover>();
   // mover->SetParameters(0.0f, 0.0f, bounds_, gType_, 0.0f, 0.0f);



//    Animation* walkAnimation = cache_->GetResource<Animation>("Models/Jack_Walk.ani");

//    AnimationState* state = modelObject->AddAnimationState(walkAnimation);
//    // The state would fail to create (return null) if the animation was not found
//    if (state)
//    {
//        // Enable full blending weight and looping
//        state->SetWeight(1.0f);
//        state->SetLooped(true);
//        state->SetTime(Random(walkAnimation->GetLength()));
//    }


    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(CatmullRom, HandleUpdate));

//    overCameraNode_ = scene_->CreateChild("OverCamera");
//    overCameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
//    Camera* overCamera = overCameraNode_->CreateComponent<Camera>();
//    overCamera->SetFarClip(1000.0f);
//    overCamera->SetNearClip(1.0f);
//    overCamera->SetOrthographic(false);
       // disable occlusion culling from it. Use the camera's
       // "view override flags" for this. We could also disable eg. shadows or force low material quality
       // if we wanted
   // overCamera->SetViewOverrideFlags(VO_DISABLE_OCCLUSION);
  //  overCameraNode_->SetPosition(Vector3(0.0f, 100.0f, -330.0f));

  //  bStart_=false;

//    pathNode = scene_->CreateChild("PathNode");
//    path = pathNode->CreateComponent<SplinePath>();

    if (scene_->GetChild ("PathNode")==nullptr)
          pathNode_ = scene_->CreateChild("PathNode");
     path_ = pathNode_->GetOrCreateComponent<SplinePath>();

     //path_->ClearControlPoints();

     scene_->CreateChild("SplineKnot "+ String(0));
     scene_->GetChild("SplineKnot "+ String(0))->SetPosition(Vector3(-50.0f, 0.0f,-50.0f));
     path_->AddControlPoint(scene_->GetChild("SplineKnot "+ String(0)), 0);

     scene_->CreateChild("SplineKnot "+ String(1));
     scene_->GetChild("SplineKnot "+ String(1))->SetPosition(Vector3(0.0f, 10.0f, 50.0f));
     path_->AddControlPoint(scene_->GetChild("SplineKnot "+ String(1)), 1);

     scene_->CreateChild("SplineKnot "+ String(2));
     scene_->GetChild("SplineKnot "+ String(2))->SetPosition(Vector3(0.0f, 10.5f, 50.0f));
     path_->AddControlPoint(scene_->GetChild("SplineKnot "+ String(2)), 2);

     scene_->CreateChild("SplineKnot "+ String(3));
     scene_->GetChild("SplineKnot "+ String(3))->SetPosition(Vector3(50.0f, 25.0f,75.0f));
     path_->AddControlPoint(scene_->GetChild("SplineKnot "+ String(3)), 3);

     path_->SetInterpolationMode( CATMULL_ROM_FULL_CURVE);

     path_->SetSpeed(10.0f);
     URHO3D_LOGINFO(String("KEYUP: path length: " + String(path_->GetLength())));
     path_->SetControlledNode(jackNode_);

     SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(CatmullRom, HandlePostRenderUpdate));

}

void CatmullRom::CreateUI()
{
    //ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    ui->Clear();
  //  uiRoot_->RemoveAllChildren();
   // window_->RemoveAllChildren();
    //window_->Remove();

    // Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will
    // control the camera, and when visible, it will point the raycast target
    XMLFile* style = cache_->GetResource<XMLFile>("UI/DefaultStyle.xml");
    SharedPtr<Cursor> cursor(new Cursor(context_));
    cursor->SetStyleAuto(style);
    ui->SetCursor(cursor);

    // Set starting position of the cursor at the rendering window center
    Graphics* graphics = GetSubsystem<Graphics>();
    cursor->SetPosition(graphics->GetWidth() / 2, graphics->GetHeight() / 2);



    // Construct new Text object, set string to display and font to use
    instructionText = ui->GetRoot()->CreateChild<Text>();

    instructionText->SetText(
        "Use WASDEF or cursor & Page Up/Down keys to move, RMB to rotate view.\n\n"
    );



    instructionText->SetFont(cache_->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
    // The text has multiple rows. Center them in relation to each other
    instructionText->SetTextAlignment(HA_CENTER);

    // Position the text relative to the screen center
    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_TOP);
    instructionText->SetPosition(0, ui->GetRoot()->GetHeight() / 16);


    uiRoot_->SetDefaultStyle(cache_->GetResource<XMLFile>("UI/DefaultStyle.xml"));
    // Let's create some text to display.
    text_ = uiRoot_->CreateChild<Text>();
    // Text will be updated later in the E_UPDATE handler. Keep readin'.
    text_->SetText("Jack Pos: X, Y \nCamera Pan, Tilt: Pan, Tilt \nCamera Pos X,Y,Z: X, Y, Z");
    // If the engine cannot find the font, it comes with Urho3D.
    // Set the environment variables URHO3D_HOME, URHO3D_PREFIX_PATH or
    // change the engine parameter "ResourcePrefixPath" in the Setup method.
    text_->SetFont(cache_->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 20);
    text_->SetColor(Color(.3f, 0.0f, .3f));
    text_->SetHorizontalAlignment(HA_CENTER);
    text_->SetVerticalAlignment(VA_BOTTOM);
  //  uiRoot_->AddChild(text_);


   // SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(CatmullRom, HandleUpdate));

}

void CatmullRom::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}


void CatmullRom::MoveCamera(float timeStep)
{
    // Right mouse button controls mouse cursor visibility: hide when pressed
    UI* ui = GetSubsystem<UI>();
    Input* input = GetSubsystem<Input>();
    ui->GetCursor()->SetVisible(!input->GetMouseButtonDown(MOUSEB_RIGHT));

    // Do not move if the UI has a focused element (the console)
   // if (ui->GetFocusElement())
  //      return;

    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

   float xyzS_ = 1.1f;
   float  xyzFactor_ = 1.1f;


    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if (input->GetKeyDown(KEY_W) || input->GetKeyDown(KEY_UP) )
        cameraNode_->Translate(xyzS_*xyzFactor_*Vector3::FORWARD * MOVE_SPEED * timeStep, TS_LOCAL);
    if (input->GetKeyDown(KEY_S) || input->GetKeyDown(KEY_DOWN)  )
        cameraNode_->Translate(xyzS_*xyzFactor_*Vector3::BACK * MOVE_SPEED * timeStep, TS_LOCAL);
    if (input->GetKeyDown(KEY_A) || input->GetKeyDown(KEY_LEFT))
        cameraNode_->Translate(xyzS_*xyzFactor_*Vector3::LEFT * MOVE_SPEED * timeStep, TS_LOCAL);
    if (input->GetKeyDown(KEY_D) || input->GetKeyDown(KEY_RIGHT))
        cameraNode_->Translate(xyzS_*xyzFactor_*Vector3::RIGHT * MOVE_SPEED * timeStep, TS_LOCAL);
    if (input->GetKeyDown(KEY_E) || input->GetKeyDown(KEY_PAGEUP))
        cameraNode_->Translate(xyzS_*xyzFactor_*Vector3::UP * MOVE_SPEED * timeStep, TS_LOCAL);
    if (input->GetKeyDown(KEY_F) || input->GetKeyDown(KEY_PAGEDOWN))
        cameraNode_->Translate(xyzS_*xyzFactor_*Vector3::DOWN * MOVE_SPEED * timeStep, TS_LOCAL);









    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    // Only move the camera when the cursor is hidden
    if (!ui->GetCursor()->IsVisible() )
    {
//        panS_ = slidersWindow_->panS_;
//        panFactor_ = slidersWindow_->panFactor_;

//        tiltS_ = slidersWindow_->tiltS_;
//        tiltFactor_ = slidersWindow_->tiltFactor_;

        IntVector2 mouseMove = input->GetMouseMove();
        yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch_ = Clamp(pitch_, -90.0f, 90.0f);

        // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
        cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
    }


}


void CatmullRom::HandleUpdate(StringHash /*eventType*/, VariantMap& eventData)
{



    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Move the camera, scale movement with time step
    //if (!bStart_)
    MoveCamera(timeStep);

    path_->Move (timeStep);


    //Jack Pos : X, Y \nCamera Pan, Tilt: Pan, Tilt \nCamera Pos X, Y: X, Y
    std::string a = "\nJack Pos : ", aVel= "\nJack Path Speed: ", b = "\nCamera Tilt, Pan:", c = "\nCamera Pos X, Y, Z:";
    std::stringstream s;
    s << std::fixed << std::setprecision(2)  << aVel << path_->GetSpeed() \
    <<  a << jackNode_->GetWorldPosition().x_ << " ," \
    << jackNode_->GetWorldPosition().y_ << " ," << jackNode_->GetWorldPosition().z_ << b << cameraNode_->GetRotation().PitchAngle() << " ," << cameraNode_->GetRotation().YawAngle() << c << cameraNode_->GetWorldPosition().x_ << " ," << cameraNode_->GetWorldPosition().y_<<" ," << cameraNode_->GetWorldPosition().z_;
    std::string str = s.str();
    String urhostr(str.c_str(), str.size());
    text_->SetText(urhostr);
   // URHO3D_LOGINFO(urhostr);     // this show how to put stuff into the log

}

void CatmullRom::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{

       path_->DrawDebugGeometry(debug_, true);
}
