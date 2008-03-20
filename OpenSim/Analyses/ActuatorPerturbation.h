#ifndef _ActuatorPerturbation_h_
#define _ActuatorPerturbation_h_
// ActuatorPerturbation.h
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	AUTHOR: Frank C. Anderson, Saryn R. Goldberg
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
* Copyright (c)  2005, Stanford University. All rights reserved. 
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
#include <OpenSim/Simulation/Model/DerivCallback.h>
#include <OpenSim/Simulation/Manager/Manager.h>
#include "osimAnalysesDLL.h"
#include "Contact.h"


//=============================================================================
//=============================================================================
/**
 * A derivatives callback used for perturbing the actuator forces during a
 * simulation.
 *
 * @author Frank C. Anderson, Saryn R. Goldberg
 * @version 1.0
 */
namespace OpenSim { 

class Model;

class OSIMANALYSES_API ActuatorPerturbation : public DerivCallback 
{
//=============================================================================
// DATA
//=============================================================================
public:
	/** Perturbation types. See setPerturbation(). */
	enum PertType {SCALE, DELTA, CONSTANT};

protected:
	/** Which actuator. */
	AbstractActuator *_actuator;
	/** Negative force flag **/
	bool _allowNegForce;
	/** Force perturbation. */
	double _perturbation;
	/** Type of perturbation */
	PertType _perturbationType;
	/** Nominal actuator force. */
	double _force;
	/** Storage for holding nominal and perturbed force. */
	Storage *_forceStore;

//=============================================================================
// METHODS
//=============================================================================
public:
	ActuatorPerturbation(Model *aModel);
	virtual ~ActuatorPerturbation();
private:
	void setNull();

public:
	//--------------------------------------------------------------------------
	// GET AND SET
	//--------------------------------------------------------------------------
	void setActuator(AbstractActuator *aActuator);
	AbstractActuator* getActuator() const;
	void setAllowNegForce(bool aTrueFalse);
	bool getAllowNegForce() const;
	void setPerturbation(PertType aPerturbationType, double aPerturbation);
	double getPerturbation() const;
	PertType getPerturbationType() const;
	Storage* getForceStorage();

	//--------------------------------------------------------------------------
	// UTILITY
	//--------------------------------------------------------------------------
	virtual void reset(); 

	//--------------------------------------------------------------------------
	// CALLBACKS
	//--------------------------------------------------------------------------
	virtual void
		computeActuation(double aT,double *aX,double *aY);
	virtual void
		applyActuation(double aT,double *aX,double *aY);

//=============================================================================
};	// END of class ActuatorPerturbation

}; //namespace
//=============================================================================
//=============================================================================


#endif // #ifndef __ActuatorPerturbation_h__
