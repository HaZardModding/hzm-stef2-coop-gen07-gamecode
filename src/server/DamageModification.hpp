//-----------------------------------------------------------------------------
//
//  $Logfile::								                                  $
// $Revision::						                                          $
//     $Date::							                                      $
//
// Copyright (C) 1999 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//

// DamageModification.hpp: interface for the Damage Modification System
//
//////////////////////////////////////////////////////////////////////

class DamageModificationSystem;
class Damage;
class DamageModifier;
class DamageModifierTikiName;
class DamageModifierName;
class DamageModifierGroup;
class DamageModifierActorType;
class DamageModifierTargetName;
class DamageModifierDamageType;

#ifndef __DAMAGEMODIFICATION_H__
#define __DAMAGEMODIFICATION_H__

#include "g_local.h"

// Damage Types enumeration
typedef enum 
	{
	UNDEFINED = 0,
	TIKI_NAME,
	NAME,
	GROUP,
	ACTOR_TYPE,
	TARGETNAME,
	DAMAGE_TYPE // MOD values
	} DamageModifierType;


//------------------------- CLASS ------------------------------
//
// Name:			DamageModificationSystem			
// Base Class:		Class
//
// Description:		The main system for damage modification
//
// Method of Use:	Used in the damage system
//--------------------------------------------------------------
class DamageModificationSystem : public Class
{
private:
	Container<DamageModifier *> damageModifiers;

public:
	DamageModificationSystem();
	virtual ~DamageModificationSystem();

	void addDamageModifier(const str &damagemodtype, const str &value, float multiplier, float chance, float painBaseLine );
	void addDamageModifier(DamageModifier *newModifier);
	void resolveDamage(Damage &damage);
	Container<DamageModifier *>& getModifierList() { return damageModifiers; }
};


//------------------------- CLASS ------------------------------
//
// Name:			Damage		
// Base Class:		Class
//
// Description:		The actual damage information that's passed around
//
// Method of Use:	Used as a parameter to resolveDamage on the main
//					DamageModificationSystem class.
//--------------------------------------------------------------
class Damage : public Class
{
public:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	float		damage = 0.0f;
	Entity		*inflictor = nullptr;
	Entity		*attacker = nullptr;
	Vector		position = Vector(0.0f, 0.0f, 0.0f);
	Vector		direction = Vector(0.0f, 0.0f, 0.0f);
	Vector		normal = Vector(0.0f, 0.0f, 0.0f);
	int			knockback = 0;
	int			dflags = 0;
	int			meansofdeath = 0;
	int			surfaceNumber = 0;
	int			boneNumber = 0;
	bool		showPain = false;
	Entity		*weapon = nullptr;


	Damage();
	Damage( Event *ev );
	virtual ~Damage();

};


//------------------------- CLASS ------------------------------
//
// Name:			DamageModifier		
// Base Class:		Class
//
// Description:		DamageModifier information (type, resistance, etc)
//
// Method of Use:	There is a container of this class in DamageModificationSystem
//--------------------------------------------------------------
class DamageModifier : public Class
{
friend class DamageModificationSystem;

private:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable _multiplier/_chance/_painBaseLine was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	float					_multiplier = 0.0f;
	float					_chance = 0.0f;
	float					_painBaseLine = 0.0f;


protected:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable _multiplier/_chance/_painBaseLine was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	DamageModifierType		_type = UNDEFINED;


	virtual void resolveDamage(Damage &damage) = 0;
	virtual void resolvePain(Damage &damage) = 0;

public:
	DamageModifier()
		: _type(UNDEFINED),
		_multiplier(0.0f)
		{ }
	DamageModifier(DamageModifierType type, float multiplier, float chance, float painBaseLine )
		: _type(type),
		_multiplier(multiplier),
		_chance(chance),
		_painBaseLine(painBaseLine)		
		{ }

	virtual ~DamageModifier() { }

	DamageModifierType getType()	{ return _type; }
	float getMultiplier()			{ return _multiplier; }
	float getPainBaseLine()			{ return _painBaseLine; }
	float getChance()				{ return _chance; }

	void Archive( Archiver &arc );
};

inline void DamageModifier::Archive(Archiver &arc)
{
	Class::Archive(arc);

	// _type is archived by the caller of this function
	//DamageModifierType		_type;

	arc.ArchiveFloat( &_multiplier );
	arc.ArchiveFloat( &_chance );
	arc.ArchiveFloat( &_painBaseLine );
}


//------------------------- CLASS ------------------------------
//
// Name:			DamageModifierTikiName	
// Base Class:		DamageModifier
//
// Description:		Sub-class to resolve damage modification
//					of tiki name type.
//
// Method of Use:	
//--------------------------------------------------------------
class DamageModifierTikiName : public DamageModifier
{
private:
	str _tikiName;

protected:
	/*virtual*/ void resolveDamage(Damage &damage);
	/*virtual*/ void resolvePain(Damage &damage);

public:
	DamageModifierTikiName() { _type = TIKI_NAME; }
	DamageModifierTikiName(DamageModifierType type, const str &tikiname, float multiplier, float chance, float painBaseLine )
		: DamageModifier(type, multiplier, chance, painBaseLine ),
		_tikiName(tikiname)
		{ }
	virtual ~DamageModifierTikiName() { }

	void Archive( Archiver &arc );
};

inline void DamageModifierTikiName::Archive(Archiver &arc)
{
	DamageModifier::Archive(arc);
	arc.ArchiveString( &_tikiName );
}

//------------------------- CLASS ------------------------------
//
// Name:			DamageModifierName	
// Base Class:		DamageModifier
//
// Description:		Sub-class to resolve damage modification
//					of name type.
//
// Method of Use:	
//--------------------------------------------------------------
class DamageModifierName : public DamageModifier
{
private:
	str _name;

protected:
	/*virtual*/ void resolveDamage(Damage &damage);
	/*virtual*/ void resolvePain(Damage &damage);

public:
	DamageModifierName() { _type = NAME; }
	DamageModifierName(DamageModifierType type, const str &name, float multiplier, float chance, float painBaseLine )
		: DamageModifier(type, multiplier, chance, painBaseLine ),
		_name(name)
		{ }
	virtual ~DamageModifierName() { }

	void Archive( Archiver &arc );
};

inline void DamageModifierName::Archive(Archiver &arc)
{
	DamageModifier::Archive(arc);
	arc.ArchiveString( &_name );
}

//------------------------- CLASS ------------------------------
//
// Name:			DamageModifierGroup	
// Base Class:		DamageModifier
//
// Description:		Sub-class to resolve damage modification
//					of group type.
//
// Method of Use:	
//--------------------------------------------------------------
class DamageModifierGroup : public DamageModifier
{
private:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable _group was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	int _group = 0;

protected:
	/*virtual*/ void resolveDamage(Damage &damage);
	/*virtual*/ void resolvePain(Damage &damage);

public:
	DamageModifierGroup() { _type = GROUP; }
	DamageModifierGroup(DamageModifierType type, int group, float multiplier, float chance, float painBaseLine )
		: DamageModifier(type, multiplier, chance, painBaseLine ),
		_group(group)
		{ }
	virtual ~DamageModifierGroup() { }
	
	void Archive( Archiver &arc );
};

inline void DamageModifierGroup::Archive(Archiver &arc)
{
	DamageModifier::Archive(arc);
	arc.ArchiveInteger( &_group );
}

//------------------------- CLASS ------------------------------
//
// Name:			DamageModifierActorType
// Base Class:		DamageModifier
//
// Description:		Sub-class to resolve damage modification
//					of actortype type.
//
// Method of Use:	
//--------------------------------------------------------------
class DamageModifierActorType : public DamageModifier
{
private:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable _actortype was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	int _actortype = 0;


protected:
	/*virtual*/ void resolveDamage(Damage &damage);
	/*virtual*/ void resolvePain(Damage &damage);

public:
	DamageModifierActorType() { _type = ACTOR_TYPE; }
	DamageModifierActorType(DamageModifierType type, int actortype, float multiplier, float chance, float painBaseLine)
		: DamageModifier(type, multiplier, chance, painBaseLine ),
		_actortype(actortype)
		{ }
	virtual ~DamageModifierActorType() { }

	void Archive( Archiver &arc );
};

inline void DamageModifierActorType::Archive(Archiver &arc)
{
	DamageModifier::Archive(arc);
	arc.ArchiveInteger( &_actortype );
}

//------------------------- CLASS ------------------------------
//
// Name:			DamageModifierTargetName
// Base Class:		DamageModifier
//
// Description:		Sub-class to resolve damage modification
//					of targetname type.
//
// Method of Use:	
//--------------------------------------------------------------
class DamageModifierTargetName : public DamageModifier
{
private:
	str _targetname;

protected:
	/*virtual*/ void resolveDamage(Damage &damage);
	/*virtual*/ void resolvePain(Damage &damage);

public:
	DamageModifierTargetName() { _type = TARGETNAME; }
	DamageModifierTargetName(DamageModifierType type, const str &targetname, float multiplier, float chance, float painBaseLine)
		: DamageModifier(type, multiplier, chance, painBaseLine ),
		_targetname(targetname)
		{ }
	virtual ~DamageModifierTargetName() { }

	void Archive( Archiver &arc );
};

inline void DamageModifierTargetName::Archive(Archiver &arc)
{
	DamageModifier::Archive(arc);
	arc.ArchiveString( &_targetname );
}

//------------------------- CLASS ------------------------------
//
// Name:			DamageModifierDamageType
// Base Class:		DamageModifier
//
// Description:		Sub-class to resolve damage modification
//					of targetname type.
//
// Method of Use:	
//--------------------------------------------------------------
class DamageModifierDamageType : public DamageModifier
{
private:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable _damagetype was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	int _damagetype = 0;

protected:
	/*virtual*/ void resolveDamage(Damage &damage);
	/*virtual*/ void resolvePain(Damage &damage);

public:
	DamageModifierDamageType() { _type = DAMAGE_TYPE; }
	DamageModifierDamageType(DamageModifierType type, int damagetype, float multiplier, float chance, float painBaseLine)
		: DamageModifier(type, multiplier, chance, painBaseLine ),
		_damagetype(damagetype)
		{ }
	virtual ~DamageModifierDamageType() { }

	void Archive( Archiver &arc );
};

inline void DamageModifierDamageType::Archive(Archiver &arc)
{
	DamageModifier::Archive(arc);
	arc.ArchiveInteger( &_damagetype );
}


#endif // !defined(__DAMAGEMODIFICATION_H__)
