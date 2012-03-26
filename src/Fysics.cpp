#include "Fysics.hpp"
#include "OBBoxRenderable.h"

#include <stdio.h>

using namespace std;

TiXmlElement* FysicsEditor::exportDotScene(TiXmlElement *pParent)
{
    Ogre::String shape = "";
    PropertyOptionsVector *option = FysicsEditorFactory::GetCollisionShapes();
    
    for(auto iter = option->begin() ; iter != option->end() ; ++iter)
    {
        if(*Ogre::any_cast<int>(&(iter->mValue)) == mShape->get())
        {
            shape = iter->mKey;
        }
    }

    // physics object 
    TiXmlElement *pPhysics = pParent->InsertEndChild(TiXmlElement("physics"))->ToElement();
    pPhysics->SetAttribute("name", mName->get().c_str());
    pPhysics->SetAttribute("id", Ogre::StringConverter::toString(mObjectID->get()).c_str());
    pPhysics->SetAttribute("shape", shape.c_str());
    pPhysics->SetAttribute("mass", Ogre::StringConverter::toString(mMass->get()).c_str());

    // gravity
    TiXmlElement *pPhysicsGravity = pPhysics->InsertEndChild(TiXmlElement("gravity"))->ToElement();
    pPhysicsGravity->SetAttribute("x", Ogre::StringConverter::toString(mGravity->get().x).c_str());
    pPhysicsGravity->SetAttribute("y", Ogre::StringConverter::toString(mGravity->get().y).c_str());
    pPhysicsGravity->SetAttribute("z", Ogre::StringConverter::toString(mGravity->get().z).c_str());

    // physics position
    TiXmlElement *pPhysicsPosition = pPhysics->InsertEndChild(TiXmlElement("position"))->ToElement();
    pPhysicsPosition->SetAttribute("x", Ogre::StringConverter::toString(mPosition->get().x).c_str());
    pPhysicsPosition->SetAttribute("y", Ogre::StringConverter::toString(mPosition->get().y).c_str());
    pPhysicsPosition->SetAttribute("z", Ogre::StringConverter::toString(mPosition->get().z).c_str());

    // physics restriction
    TiXmlElement *pPhysicsRestrictMove = pPhysics->InsertEndChild(TiXmlElement("restrictMove"))->ToElement();
    pPhysicsRestrictMove->SetAttribute("x", Ogre::StringConverter::toString(mMoveRestrict->get().x).c_str());
    pPhysicsRestrictMove->SetAttribute("y", Ogre::StringConverter::toString(mMoveRestrict->get().y).c_str());
    pPhysicsRestrictMove->SetAttribute("z", Ogre::StringConverter::toString(mMoveRestrict->get().z).c_str());
    
    TiXmlElement *pPhysicsRestrictRotate = pPhysics->InsertEndChild(TiXmlElement("restrictRotate"))->ToElement();
    pPhysicsRestrictRotate->SetAttribute("x", Ogre::StringConverter::toString(mRotateRestrict->get().x).c_str());
    pPhysicsRestrictRotate->SetAttribute("y", Ogre::StringConverter::toString(mRotateRestrict->get().y).c_str());
    pPhysicsRestrictRotate->SetAttribute("z", Ogre::StringConverter::toString(mRotateRestrict->get().z).c_str());

    return pPhysics;
}


Ogre::AxisAlignedBox FysicsEditor::getAABB()
{
    if(mEntity)
    {
        return mEntity->getBoundingBox();
    } 
    else 
        return Ogre::AxisAlignedBox::BOX_NULL;
}

void FysicsEditor::setDerivedPosition(Ogre::Vector3 val)
{
    if(getParent())
    {
        Ogre::Quaternion qParent = getParent()->getDerivedOrientation().Inverse();
        Ogre::Vector3 vParent = getParent()->getDerivedPosition();
        Ogre::Vector3 newPos = (val - vParent);
        val = qParent * newPos;
    }
    mPosition->set(val);
}

Ogre::Vector3 FysicsEditor::getDerivedPosition()
{
    if(mHandle)
        return mHandle->_getDerivedPosition();
    else
        return getParent()->getDerivedPosition() + (getParent()->getDerivedOrientation() * mPosition->get());
}

void FysicsEditor::setSelectedImpl(bool bSelected)
{
    CBaseEditor::setSelectedImpl(bSelected);

    showBoundingBox(bSelected);
}

bool FysicsEditor::setHighlightedImpl(bool highlight )
{
    showBoundingBox(highlight);

    return true;
}

void FysicsEditor::showBoundingBox(bool bShow)
{
    if(!mBoxParentNode)
        createBoundingBox();

    if(mBBoxNode)
    {
        int matpos = 0;
        if(mHighlighted->get())
        {
            ++matpos;
            if(mSelected->get())
                ++matpos;
        }
        
        mOBBoxRenderable->setMaterial(mOBBMaterials[matpos]);
        mBBoxNode->setVisible(( bShow || mSelected->get() || mHighlighted->get() ));
    }
}

void FysicsEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mPosition, "position",Ogre::Vector3,Ogre::Vector3::ZERO,0,SETTER(Ogre::Vector3, FysicsEditor, _setPosition));
    PROPERTY_PTR(mShape, "shape", int, 0, 0, SETTER(int, FysicsEditor, _setShape));
    PROPERTY_PTR(mMass, "mass", Ogre::Real, 1.f, 0, SETTER(Ogre::Real, FysicsEditor, _setMass));
    PROPERTY_PTR(mGravity, "gravity", Ogre::Vector3, Ogre::Vector3::UNIT_Y, 0, SETTER(Ogre::Vector3, FysicsEditor, _setProperty));
    PROPERTY_PTR(mMoveRestrict, "restrictMove", Ogre::Vector3, Ogre::Vector3(1, 1, 1), 0, SETTER(Ogre::Vector3, FysicsEditor, _setProperty));
    PROPERTY_PTR(mRotateRestrict, "restrictRotate", Ogre::Vector3, Ogre::Vector3(1, 1, 1), 0, SETTER(Ogre::Vector3, FysicsEditor, _setProperty));
    mProperties.initValueMap(params);
}

bool FysicsEditor::_setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position)
{
    if(mHandle)
    {
      mHandle->setPosition(position);
    }

    _updatePaging();

    return true;
}

bool FysicsEditor::_setShape(OgitorsPropertyBase* property, const int& shape) {
    PropertyOptionsVector* options = FysicsEditorFactory::GetCollisionShapes();

    for(auto iter = options->begin() ; iter != options->end() ; ++iter)
    {
        if(*Ogre::any_cast<int>(&(iter->mValue)) == shape)
            return true;
    }

    return true;
}

bool FysicsEditor::_setMass(OgitorsPropertyBase* property, const Ogre::Real& mass)
{
    return true;
}

bool FysicsEditor::_setProperty(OgitorsPropertyBase* property, const Ogre::Vector3& vec3)
{
    return true;
}

void FysicsEditor::setParentImpl(CBaseEditor *oldparent, CBaseEditor *newparent)
{
    if(!mHandle)
        return;

    if(oldparent) 
        oldparent->getNode()->removeChild(mHandle);
    
    if(newparent) 
        newparent->getNode()->addChild(mHandle);
}

bool FysicsEditor::load(bool async)
{
    cout << mName->get() << endl;
    if(mLoaded->get())
        return true;
    if(!getParent()->load())
        return false;

    //Create Physics Object
    mEntity = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(), "Fysics.mesh");

    mHandle = getParent()->getNode()->createChildSceneNode(mName->get());
    mHandle->attachObject(mEntity);

    mHandle->setPosition(mPosition->get());

    mLoaded->set(true);

    return true;
}

bool FysicsEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    destroyBoundingBox();

    if(mHandle)
    {
        Ogre::Node *parent = mHandle->getParent();
        parent->removeChild(mHandle);
        mOgitorsRoot->GetSceneManager()->destroySceneNode(mHandle);
        mOgitorsRoot->GetSceneManager()->destroyEntity(mEntity);
        mHandle = 0;
        mEntity = 0;
    }
    
    mLoaded->set(false);
    return true;
}

FysicsEditor::FysicsEditor(CBaseEditorFactory *factory)
    : CBaseEditor(factory)
{
    mHandle = 0;
    mPosition = 0;
    mShape = 0;
    mMass = 0;
    mGravity = 0;
    mMoveRestrict = 0;
    mRotateRestrict = 0;
    mUsesGizmos = true;
}

FysicsEditor::~FysicsEditor()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
PropertyOptionsVector FysicsEditorFactory::mCollisionShapes;

FysicsEditorFactory::FysicsEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Physics Object";
    mEditorType = ETYPE_NODE;
    mAddToObjectList = true;
    mRequirePlacement = true;
    mIcon = "Icons/Fysics.svg";//----------------
    mCapabilities = CAN_MOVE|CAN_DELETE|CAN_CLONE|CAN_FOCUS|CAN_DRAG|CAN_ACCEPTCOPY;

    mCollisionShapes.clear();
    mCollisionShapes.push_back(PropertyOption("BOX", Ogre::Any((int)FysicsEditor::BOX)));
    mCollisionShapes.push_back(PropertyOption("CONVEX", Ogre::Any((int)FysicsEditor::CONVEX)));
    mCollisionShapes.push_back(PropertyOption("SPHERE", Ogre::Any((int)FysicsEditor::SPHERE)));
    mCollisionShapes.push_back(PropertyOption("CYLINDER", Ogre::Any((int)FysicsEditor::CYLINDER)));
    mCollisionShapes.push_back(PropertyOption("TRIMESH", Ogre::Any((int)FysicsEditor::TRIMESH)));

    AddPropertyDefinition("position","Position","The position of the object.",PROP_VECTOR3);
    AddPropertyDefinition("shape", "Collision Shape", "The shape of the physics collision body. Value can be : BOX CONVEX SHERE CYLINDER TRIMESH .",
        PROP_INT)->setOptions(&mCollisionShapes);
    AddPropertyDefinition("mass","Mass","The mass of the object.",PROP_REAL);
    AddPropertyDefinition("gravity","Gravity","The gravity direction and power of the object.",PROP_VECTOR3);
    AddPropertyDefinition("restrictMove","Movement Restrict","Which axis the object physics movement is restricted. Value can be 0 for no movement, or 1 for movement.",PROP_VECTOR3);
    AddPropertyDefinition("restrictRotate","Rotation Restrict","Which axis the object physics rotation is restricted. Value can be 0 for no rotation, or 1 for rotation.",PROP_VECTOR3);
}

CBaseEditorFactory *FysicsEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW FysicsEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}

CBaseEditor *FysicsEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    FysicsEditor *object = OGRE_NEW FysicsEditor(this);
    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("Physics"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);
    object->registerForPostSceneUpdates();

    mInstanceCount++;
    return object;
}

Ogre::String FysicsEditorFactory::GetPlaceHolderName()
{
    return "Fysics.mesh";
}

PropertyOptionsVector* FysicsEditorFactory::GetCollisionShapes()
{
    return &mCollisionShapes;
}
/////////////////////////////////////////////////////////////////////////
FysicsEditorFactory *fysicsFac = 0;

bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "Fysics Object";
    fysicsFac = OGRE_NEW FysicsEditorFactory();
    OgitorsRoot::getSingletonPtr()->RegisterEditorFactory(identifier, fysicsFac);
    return true;
}

bool dllStopPlugin(void)
{
    OGRE_DELETE fysicsFac;
    fysicsFac = 0;
    return true;
}
