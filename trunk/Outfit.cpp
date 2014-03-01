/* Outfit.cpp
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "Outfit.h"

#include "SpriteSet.h"

using namespace std;



Outfit::Outfit()
	: thumbnail(nullptr), ammo(nullptr), icon(nullptr)
{
}



void Outfit::Load(const DataFile::Node &node, const Set<Outfit> &outfits, const Set<Effect> &effects)
{
	if(node.Size() >= 2)
		name = node.Token(1);
	category = "Other";
	
	for(const DataFile::Node &child : node)
	{
		if(child.Token(0) == "category" && child.Size() >= 2)
			category = child.Token(1);
		else if(child.Token(0) == "flare sprite" && child.Size() >= 2)
			flare.Load(child);
		else if(child.Token(0) == "thumbnail" && child.Size() >= 2)
			thumbnail = SpriteSet::Get(child.Token(1));
		else if(child.Token(0) == "weapon")
		{
			for(const DataFile::Node &grand : child)
			{
				if(grand.Token(0) == "sprite" && grand.Size() >= 2)
					weaponSprite.Load(grand);
				else if(grand.Token(0) == "ammo" && grand.Size() >= 2)
					ammo = outfits.Get(grand.Token(1));
				else if(grand.Token(0) == "icon" && grand.Size() >= 2)
					icon = SpriteSet::Get(grand.Token(1));
				else if(grand.Token(0) == "hit effect" && grand.Size() >= 2)
				{
					int count = (grand.Size() >= 3) ? grand.Value(2) : 1;
					hitEffects[effects.Get(grand.Token(1))] += count;
				}
				else if(grand.Token(0) == "die effect" && grand.Size() >= 2)
				{
					int count = (grand.Size() >= 3) ? grand.Value(2) : 1;
					dieEffects[effects.Get(grand.Token(1))] += count;
				}
				else if(grand.Token(0) == "submunition" && grand.Size() >= 2)
				{
					int count = (grand.Size() >= 3) ? grand.Value(2) : 1;
					submunitions[outfits.Get(grand.Token(1))] += count;
				}
				else if(grand.Size() >= 2)
					weapon[grand.Token(0)] = grand.Value(1);
			}
			weapon["range"] = weapon["lifetime"] * (
				weapon["velocity"] + .5 * weapon["acceleration"] * weapon["lifetime"]);
		}
		else if(child.Size() >= 2)
			attributes[child.Token(0)] = child.Value(1);
	}
}



const string &Outfit::Name() const
{
	return name;
}



const string &Outfit::Category() const
{
	return category;
}



int Outfit::Cost() const
{
	return Get("cost");
}



// Get the image to display in the outfitter when buying this item.
const Sprite *Outfit::Thumbnail() const
{
	return thumbnail;
}



double Outfit::Get(const string &attribute) const
{
	auto it = attributes.find(attribute);
	return (it == attributes.end()) ? 0. : it->second;
}



const map<string, double> &Outfit::Attributes() const
{
	return attributes;
}



// Determine whether the given number of instances of the given outfit can
// be added to a ship with the attributes represented by this instance. If
// not, return the maximum number that can be added.
int Outfit::CanAdd(const Outfit &other, int count) const
{
	for(const auto &at : other.attributes)
	{
		double value = Get(at.first);
		if(value + at.second * count < 0.)
			count = value / -at.second;
	}
	
	return count;
}



// For tracking a combination of outfits in a ship: add the given number of
// instances of the given outfit to this outfit.
void Outfit::Add(const Outfit &other, int count)
{
	for(const auto &at : other.attributes)
		attributes[at.first] += at.second * count;
	
	if(other.flare.GetSprite())
		flare = other.flare;
}



// Modify this outfit's attributes.
void Outfit::Add(const string &attribute, double value)
{
	attributes[attribute] += value;
}


	
// Get this outfit's engine flare sprite, if any.
const Animation &Outfit::FlareSprite() const
{
	return flare;
}



bool Outfit::IsWeapon() const
{
	return !weapon.empty();
}



// Get the weapon provided by this outfit, if any.
const Animation &Outfit::WeaponSprite() const
{
	return weaponSprite;
}



const Outfit *Outfit::Ammo() const
{
	return ammo;
}



const Sprite *Outfit::Icon() const
{
	return icon;
}



double Outfit::WeaponGet(const string &attribute) const
{
	auto it = weapon.find(attribute);
	return (it == weapon.end()) ? 0. : it->second;
}



// Handle a weapon impacting something or reaching its end of life.
const map<const Effect *, int> Outfit::HitEffects() const
{
	return hitEffects;
}



const map<const Effect *, int> Outfit::DieEffects() const
{
	return dieEffects;
}



const map<const Outfit *, int> Outfit::Submunitions() const
{
	return submunitions;
}
