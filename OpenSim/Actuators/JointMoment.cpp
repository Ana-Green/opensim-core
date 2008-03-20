// JointMoment.cpp
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	AUTHORS:  Frank C. Anderson
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
* Copyright (c)  2005, Stanford University, All rights reserved. 
* Use of the OpenSim software in source form is permitted provided that the following
* conditions are met:
* 	1. The software is used only for non-commercial research and education. It may not
*     be used in relation to any commercial activity.
* 	2. The software is not distributed or redistributed.  Software distribution is allowed 
*     only through https://simtk.org/home/opensim.
* 	3. Use of the OpenSim software or derivatives must be acknowledged in all publications,
*      presentations, or documents describing work in which OpenSim or derivatives are used.
* 	4. Credits to developers may not be removed from executables
*     created from modifications of the source.
* 	5. Modifications of source code must retain the above copyright notice, this list of
*     conditions and the following disclaimer. 
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
*  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
*  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR BUSINESS INTERRUPTION) OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
*  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


//=============================================================================
// INCLUDES
//=============================================================================
#include <OpenSim/Common/rdMath.h>
#include <OpenSim/Common/PropertyDbl.h>
#include "JointMoment.h"
#include "Muscle.h"
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/Model/AbstractDynamicsEngine.h>
#include <OpenSim/Simulation/Model/AbstractCoordinate.h>
#include <OpenSim/Simulation/Model/SpeedSet.h>




using namespace OpenSim;
using namespace std;


//=============================================================================
// STATICS
//=============================================================================


//=============================================================================
// CONSTRUCTOR(S) AND DESTRUCTOR
//=============================================================================
//_____________________________________________________________________________
/**
 * Destructor.
 */
JointMoment::~JointMoment()
{
}
//_____________________________________________________________________________
/**
 * Default constructor.
 */
JointMoment::
JointMoment(string aQName) :
	GeneralizedForceAtv(aQName),
	_optimalNegForce(_propOptimalNegForce.getValueDbl())
{
	setNull();
}
//_____________________________________________________________________________
/**
 * Copy constructor.
 *
 * @param aActuator Actuator to be copied.
 */
JointMoment::
JointMoment(const JointMoment &aActuator) :
	GeneralizedForceAtv(aActuator),
	_optimalNegForce(_propOptimalNegForce.getValueDbl())
{
	setNull();
	copyData(aActuator);
}
//_____________________________________________________________________________
/**
 * Copy this actuator and return a pointer to the copy.
 * The copy constructor for this class is used.
 *
 * @return Pointer to a copy of this actuator.
 */
Object* JointMoment::
copy() const
{
	AbstractActuator *act = new JointMoment(*this);
	return(act);
}


//=============================================================================
// CONSTRUCTION
//=============================================================================
//_____________________________________________________________________________
/**
 * Set the data members of this actuator to their null values.
 */
void JointMoment::
setNull()
{
	setType("JointMoment");
	setupProperties();

	setNumControls(1); setNumStates(1); setNumPseudoStates(0);
	bindControl(0, _excitation, "excitation");
	bindState(0, _a, "activation");

	// APPLIES FORCE
	_appliesForce = true;
}
//_____________________________________________________________________________
/**
 * Set up the serializable member variables.  This involves generating
 * properties and connecting local variables to those properties.
 */
void JointMoment::
setupProperties()
{
	_propOptimalNegForce.setName("optimal_negative_force");
	_propOptimalNegForce.setValue(1.0);
	_propertySet.append( &_propOptimalNegForce );
}

//_____________________________________________________________________________
/**
 * Copy the member data of the specified actuator.
 */
void JointMoment::
copyData(const JointMoment &aActuator)
{
	setOptimalNegativeForce(aActuator.getOptimalNegativeForce());
}


//=============================================================================
// OPERATORS
//=============================================================================
//-----------------------------------------------------------------------------
// ASSIGNMENT
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
/**
 * Assignment operator.
 *
 * @return  Reference to the altered object.
 */
JointMoment& JointMoment::
operator=(const JointMoment &aActuator)
{
	// BASE CLASS
	GeneralizedForceAtv::operator =(aActuator);

	// DATA
	copyData(aActuator);

	return(*this);
}


//=============================================================================
// GET AND SET
//=============================================================================
//-----------------------------------------------------------------------------
// OPTIMAL NEGATIVE FORCE
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
/**
 * Set the optimal negative force.  This property is used when the activation
 * level of the actuator is negative, allowing for a different flexion and
 * extension stregths of the actuator.
 *
 * Note that this property should always be positive.  If a negative value
 * is sent in through the argument list of this method, it's absolute value
 * is taken.
 *
 * @param aOptimalNegForce Optinal negative force of the actuator.
 */
void JointMoment::
setOptimalNegativeForce(double aOptimalNegForce)
{
	_optimalNegForce = fabs(aOptimalNegForce);

}
//_____________________________________________________________________________
/**
 * Get the optimal negative force.  This property is used when the activation
 * level of the actuator is negative, allowing for a different flexion and
 * extension stregths of the actuator.
 *
 * This property should always be positive.
 *
 * @return Optinal negative force of the actuator.
 */
double JointMoment::
getOptimalNegativeForce() const
{
	return(_optimalNegForce);
}


//=============================================================================
// COMPUTATIONS
//=============================================================================
//_____________________________________________________________________________
/**
 * Compute all quantities necessary for applying the actuator force to the
 * model.
 */
void JointMoment::
computeActuation()
{
	if(_model==NULL) return;

	// SPEED
	if (_q)
	{
		AbstractSpeed *speed =  _model->getDynamicsEngine().getSpeedSet()->get(_q->getName());
		if (speed)
			_speed = speed->getValue();
	}

	// FORCE
	double force;
	if(_a<0.0) {
		force = _a * _optimalNegForce;
	} else {
		force = _a * _optimalForce;
	}

	setForce(force);
}


//=============================================================================
// XML
//=============================================================================
//-----------------------------------------------------------------------------
// UPDATE FROM XML NODE
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
/**
 * Update this object based on its XML node.
 *
 * This method simply calls Object::updateFromXMLNode() and then calls
 * a few methods in this class to ensure that variable members have been
 * set in a consistent manner.
 */
void JointMoment::
updateFromXMLNode()
{
	GeneralizedForceAtv::updateFromXMLNode();
	setOptimalNegativeForce(_optimalNegForce);
}	

