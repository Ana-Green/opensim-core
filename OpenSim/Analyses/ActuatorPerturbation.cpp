// ActuatorPerturbation.cpp
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	AUTHOR: Frank C. Anderson, Saryn Goldberg, and May Liu
// 
// NOTE:  When you use this callBack to make a perturbation the actuator force,
//        this change in the force IS NOT recoreded in the state file.  If you
//			 want to run an induced accleration analysis using results from a 
//			 perturbation, you must first alter the states fiel to accurately
//			 reflect the changes made to the forces.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Copyright (c)  2006 Stanford University
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <OpenSim/Common/rdMath.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/Model/AbstractActuator.h>
#include "ActuatorPerturbation.h"

//=============================================================================
// CONSTANTS
//=============================================================================



//=============================================================================
// CONSTRUCTOR(S) AND DESTRUCTOR
//=============================================================================
//_____________________________________________________________________________


using namespace OpenSim;
/**
 * Destructor.
 */
ActuatorPerturbation::~ActuatorPerturbation()
{
}
//_____________________________________________________________________________
/**
 * Construct a derivative callback instance for perturbing actuator forces
 * during an integration.
 *
 * @param aModel Model for which actuator forces are to be perturbed.
 */
ActuatorPerturbation::
ActuatorPerturbation(Model *aModel) :
	DerivCallback(aModel)
{
	setNull();

	// BASE-CLASS MEMBER VARIABLES
	setType("ActuatorPerturbation");
}


//=============================================================================
// CONSTRUCTION METHODS
//=============================================================================
//_____________________________________________________________________________
/**
 * Set member variables to approprate NULL values.
 */
void ActuatorPerturbation::
setNull()
{
	_actuator = NULL;
	_allowNegForce = true;
	_perturbation = 0.0;
	_perturbationType = SCALE;
	_force = 0.0;
	_forceStore = new Storage();
	_forceStore->setName("forceStore");
	Array<std::string> labels;
	labels.append("time");
	labels.append("nominal");
	labels.append("perturbed");
	_forceStore->setColumnLabels(labels);
}

//=============================================================================
// GET AND SET
//=============================================================================
//-----------------------------------------------------------------------------
// ACTUATOR
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
/**
 * Set for which actuator a force perturbation should be made.
 *
 * @param aAct Pointer to the actuator.
 */
void ActuatorPerturbation::
setActuator(AbstractActuator *aAct)
{
	_actuator = aAct;
}
//_____________________________________________________________________________
/**
 * Get for which actuator a force perturbation should be made.
 *
 * @return Pointer to the actuator.
 */
AbstractActuator* ActuatorPerturbation::
getActuator() const
{
	return(_actuator);
}

//-----------------------------------------------------------------------------
// ALLOW NEGATIVE FORCE FLAG
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
/**
 * Set whether it is permissable that a perturbation produces a negative force
 *
 * @param allowNegForce Flag indicates whether a negative force is permitted
 */
void ActuatorPerturbation::
setAllowNegForce(bool aTrueFalse)
{
	_allowNegForce = aTrueFalse;
}
//_____________________________________________________________________________
/**
 * Get whether negative force is allowed
 *
 * @return allowNegForce Flag
 * @see setAllowNegForce()
 */
bool ActuatorPerturbation::
getAllowNegForce() const
{
	return(_allowNegForce);
}

//-----------------------------------------------------------------------------
// PERTURBATION
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
/**
 * Set the perturbation type and the perturbation size.
 *
 * Three different types of perturbation are allowed, as listed in the enum
 * PertType.
 *
 * PertType SCALE\n
 * The force perturbation (df) that is made is computed by multiplying
 * the action force (f) by the perturbation:\n
 *		df = perturbation * f
 *
 * The perturbation factor may be any value; however, a value between 0.0 and
 * 1.0 what is typically used.
 *
 * PertType DELTA\n
 * The force perturbation (df) that is made is computed by adding the perturbation 
 * to the the action force (f) by the perturbation:\n
 *		df = f + perturbation
 *
 * PertType CONSTANT\n
 * The force perturbation (df) that is made is computed by seting the 
 * action force (f) equal to the perturbation:\n
 *		df = perturbation
 *
 * @param aPerturbationType Perturbation type.
 * @param aPerturbation Perturbation value.
 */
void ActuatorPerturbation::
setPerturbation(PertType aPerturbationType, double aPerturbation)
{
	_perturbationType = aPerturbationType;
	_perturbation = aPerturbation;
}
//_____________________________________________________________________________
/**
 * Get the size of the force perturbation that is made to the
 * action forces.
 *
 * @return aPerturbation 
 */
double ActuatorPerturbation::
getPerturbation() const
{
	return(_perturbation);
}
//_____________________________________________________________________________
/**
 * Get the type of the force perturbation that is made to the
 * action forces.
 *
 * @return aPerturbationType 
 */
ActuatorPerturbation::PertType ActuatorPerturbation::
getPerturbationType() const
{
	return(_perturbationType);
}

//-----------------------------------------------------------------------------
// FORCE STORAGE
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
/**
 * Get a pointer to the Storage containing the nominal and perturbed forces.
 *
 * @return Pointer to the force storage. 
 */
Storage* ActuatorPerturbation::
getForceStorage()
{
	return(_forceStore);
}


//=============================================================================
// CALLBACKS
//=============================================================================
//-----------------------------------------------------------------------------
// RESET
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
/**
 * Reset the perturbation callback.
 *
 * This method resets any internal objects so that a new perturbation series
 * can be run cleanly.

 * Specifically, for this class, the force storage object is reset (or
 * emptied) so that any new force perturbations are stored at the beginning
 * of the force storage object.
 */
void ActuatorPerturbation::
reset()
{
	_forceStore->reset();
}


//=============================================================================
// CALLBACKS
//=============================================================================
//_____________________________________________________________________________
/**
 * Callback called right after actuation has been computed by the model.
 *
 * The nominal atuator force is recorded so that it can be restored, and the
 * actuator force is replaced by its perturbed value.
 *
 * @param aT Real time.
 * @param aX Controls.
 * @param aY States.
 */
void ActuatorPerturbation::
computeActuation(double aT,double *aX,double *aY)
{
	if(_model==NULL) {
		printf("ActuatorPerturbation.computeActuation: WARN- no model.\n");
		return;
	}
	if(!getOn()) return;

	// RECORD NOMINAL FORCE
	_force = _actuator->getForce();

	// COMPUTE PERTURBED FORCE
	int index;
	double forceArray[2];
	double force;
	force = _force;
	//printf("aT=%lf  start=%lf   end=%lf\n",aT,getStartTime(),getEndTime());
	if((aT>=getStartTime()) && (aT<getEndTime())){

	//	printf("perturbing\n");

		// SCALE
		if( _perturbationType == SCALE)	{
			force = _force + _perturbation*_force;

		// DELTA
		} else if( _perturbationType == DELTA)	{
			force = _force + _perturbation;

		// CONSTANT
		} else if( _perturbationType == CONSTANT)	{
			force = _perturbation;

		// UNRECOGNIZED
		} else {
			printf("ActuatorPerturbation.computeActuation:- unrecognized perturbationType.\n");
			force = _force;
		}

//		printf("nominal force: %f\t new force: %f\n",_force,force);


	}
	
	// MAKE CORRECTION IF PERTRUBED FORCE IS NEGATIVE AND NEG FORCE FLAG IS SET
	if(_allowNegForce==false && force < 0.0)
		force = 0.0;

	forceArray[0] = _force;
	forceArray[1] = force;

	//printf("time = %lf\toriginal force = %lf\n",aT*_model->getTimeNormConstant(),_force);
	//printf("perturbed force = %lf\n",force);
	index = _forceStore->append(aT*_model->getTimeNormConstant(),2,forceArray);

	// SET PERTURBED FORCE
	_actuator->setForce(force);
}
//_____________________________________________________________________________
/**
 * Callback called right after actuation has been applied by the model.
 *
 * The nominal atuator force is restored.
 */
void ActuatorPerturbation::
applyActuation(double aT,double *aX,double *aY)
{
	if(_model==NULL) {
		printf("ActuatorPerturbation.applyActuation: WARN- no model.\n");
		return;
	}
	if(!getOn()) return;

	// RESTORE NOMINAL FORCE
	_actuator->setForce(_force);
}





