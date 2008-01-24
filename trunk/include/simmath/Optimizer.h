
#ifndef _SimTK_OPTIMIZER_H
#define _SimTK_OPTIMIZER_H

/* Portions copyright (c) 2006 Stanford University and Jack Middleton.
 * Contributors:
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "SimTKcommon.h"
#include "SimTKmath.h"
#include <limits.h>

namespace SimTK {

enum OptimizerAlgorithm {
     BestAvailiable = 0, // Simmath will select best Optimizer based on problem type
     InteriorPoint  = 1, // IPOPT interior point optimizer
     LBFGS          = 2, // LBFGS optimizer
     LBFGSB         = 3, // LBFGS optimizer with simple bounds
     CFSQP          = 4  // CFSQP sequential quadratic programming optimizer (requires external library)
};
/**
 * The OptimizerSystem class describes the system to be optimized.
 */

class SimTK_SIMMATH_EXPORT OptimizerSystem {
public:
    OptimizerSystem() : numParameters(0),
                        numEqualityConstraints(0),
                        numInequalityConstraints(0),
                        numLinearEqualityConstraints(0),
                        numLinearInequalityConstraints(0),
                        useLimits( false ),
                        lowerLimits(0),
                        upperLimits(0) { 
    }

    OptimizerSystem(int nParameters ) : numEqualityConstraints(0),
                                      numInequalityConstraints(0),
                                      numLinearEqualityConstraints(0),
                                      numLinearInequalityConstraints(0),
                                      useLimits( false ),
                                      lowerLimits(0),
                                      upperLimits(0) { 
        setNumParameters(nParameters);
    }

  virtual ~OptimizerSystem() {
     if( useLimits ) {
         delete lowerLimits;
         delete upperLimits;
     }
  }

  /* this method must be supplied by concreate class */
  virtual int objectiveFunc      ( const Vector& parameters, 
                                 const bool new_parameters, Real& f ) const {
                                 SimTK_THROW2(SimTK::Exception::UnimplementedVirtualMethod , "OptimizerSystem", "objectiveFunc" );
                                 return -1; }


  virtual int gradientFunc       ( const Vector &parameters, 
                                 const bool new_parameters, Vector &gradient ) const  {
                                 SimTK_THROW2(SimTK::Exception::UnimplementedVirtualMethod , "OptimizerSystem", "gradientFunc" );
                                 return -1; }

  virtual int constraintFunc     ( const Vector & parameters, 
                                 const bool new_parameters, Vector & constraints ) const {
                                 SimTK_THROW2(SimTK::Exception::UnimplementedVirtualMethod , "OptimizerSystem", "constraintFunc" );
                                 return -1; }

  virtual int constraintJacobian ( const Vector& parameters, 
                                  const bool new_parameters, Matrix& jac ) const {
                                 SimTK_THROW2(SimTK::Exception::UnimplementedVirtualMethod , "OptimizerSystem", "constraintJacobian" );
                                 return -1; }

  virtual int hessian            (  const Vector &parameters, 
                                 const bool new_parameters, Vector &gradient) const {
                                 SimTK_THROW2(SimTK::Exception::UnimplementedVirtualMethod , "OptimizerSystem", "hessian" );
                                 return -1; }

   virtual void setNumParameters( const int nParameters ) {

       if(   nParameters < 1 ) {
           const char* where = " OptimizerSystem  Constructor";
           const char* szName = "number of parameters";
           SimTK_THROW5(SimTK::Exception::ValueOutOfRange, szName, 1, nParameters, INT_MAX, where);
       }else {
            numParameters = nParameters;
       }
   }
   void setNumEqualityConstraints( const int n ) {
 
        if( n < 0 ) {
           const char* where = " OptimizerSystem  setNumEqualityConstraints";
           const char* szName = "number of equality constraints";
           SimTK_THROW3(SimTK::Exception::SizeWasNegative, szName, n, where);
        } else {
           numEqualityConstraints = n;
        }
   }
   void setNumInequalityConstraints( const int n ) {

       if( n < 0 ) {
           const char* where = " OptimizerSystem  setNumInequalityConstraints";
           const char* szName = "number of inequality constraints";
           SimTK_THROW3(SimTK::Exception::SizeWasNegative, szName, n, where);
       }else {
            numInequalityConstraints = n;
       }
   }
   void setNumLinearEqualityConstraints( const int n ) {
 
        if( n < 0 || n > numEqualityConstraints ) {
           const char* where = " OptimizerSystem  setNumLinearEqualityConstraints";
           const char* szName = "number of linear equality constraints";
           SimTK_THROW4(SimTK::Exception::SizeOutOfRange, szName, n, numEqualityConstraints, where);
        } else {
           numLinearEqualityConstraints = n;
        }
   }
   void setNumLinearInequalityConstraints( const int n ) {

       if( n < 0 || n > numInequalityConstraints ) {
           const char* where = " OptimizerSystem  setNumLinearInequalityConstraints";
           const char* szName = "number of linear inequality constraints";
           SimTK_THROW4(SimTK::Exception::SizeOutOfRange, szName, n, numInequalityConstraints, where);
       }else {
            numLinearInequalityConstraints = n;
       }
   }
   void setParameterLimits( const Vector& lower, const Vector& upper  ) {
 
        if(   upper.size() != numParameters  && upper.size() != 0) {
           const char* where = " OptimizerSystem  setParamtersLimits";
           const char* szName = "upper limits length";
           SimTK_THROW5(Exception::IncorrectArrayLength, szName, upper.size(), "numParameters", numParameters, where);
        }
        if(   lower.size() != numParameters  && lower.size() != 0 ) {
           const char* where = " OptimizerSystem  setParamtersLimits";
           const char* szName = "lower limits length";
           SimTK_THROW5(Exception::IncorrectArrayLength, szName, lower.size(), "numParameters", numParameters, where);
        } 
       // set the upper and lower limits
       if( useLimits ) {
           delete lowerLimits;
           delete upperLimits;
       }

       if( upper.size() == 0 ) {
          useLimits = false;
       } else {
          lowerLimits = new Vector( lower );
          upperLimits = new Vector( upper );
          useLimits = true;
       }

       return;
   }
   int getNumParameters() const {return numParameters;}

   int getNumConstraints() const {return numEqualityConstraints+numInequalityConstraints;}
   int getNumEqualityConstraints() const {return numEqualityConstraints;}
   int getNumInequalityConstraints() const {return numInequalityConstraints;}
   int getNumLinearEqualityConstraints() const {return numLinearEqualityConstraints;}
   int getNumNonlinearEqualityConstraints() const {return numEqualityConstraints-numLinearEqualityConstraints;}
   int getNumLinearInequalityConstraints() const {return numLinearInequalityConstraints;}
   int getNumNonlinearInequalityConstraints() const {return numInequalityConstraints-numLinearInequalityConstraints;}

   bool getHasLimits() const { return useLimits; }
   void getParameterLimits( double **lower, double **upper ) const {
        *lower = &(*lowerLimits)[0];
        *upper = &(*upperLimits)[0];
   }


   int numParameters;
   int numEqualityConstraints;
   int numInequalityConstraints;
   int numLinearEqualityConstraints;
   int numLinearInequalityConstraints;
   bool useLimits;
   Vector* lowerLimits;
   Vector* upperLimits;

}; // class OptimizerSystem

// These static functions are private to the current (client-side) compilation
// unit. They are used to navigate the client-side OptimizerSystem virtual function
// table, which cannot be done on the library side. Note that these are defined
// in the SimTK namespace so don't need "SimTK" in their names.
static int objectiveFunc_static(const OptimizerSystem& sys,
                                 const Vector& parameters, 
                                const bool new_parameters, Real& f ) {

    return sys.objectiveFunc(parameters, new_parameters,  f);
}
static int gradientFunc_static(const OptimizerSystem& sys,
                                const Vector &parameters, 
                               const bool new_parameters, Vector &gradient ) {
    return sys.gradientFunc( parameters, new_parameters, gradient);
}
static int constraintFunc_static(const OptimizerSystem& sys,
                                 const Vector &parameters, 
                                 const bool new_parameters, Vector& constraints ) {
    return sys.constraintFunc( parameters, new_parameters, constraints);
}
static int constraintJacobian_static(const OptimizerSystem& sys,
                                 const Vector &parameters, 
                                 const bool new_parameters, Matrix& jac ) {
    return sys.constraintJacobian( parameters, new_parameters, jac);
}
static int hessian_static(const OptimizerSystem& sys,
                                 const Vector &parameters,
                                 const bool new_parameters, Vector& gradient ) {
    return sys.hessian( parameters, new_parameters, gradient);
}

/**
 * API for Simmath's optimizers.
 * An optimizer finds a local minimum to an objective function. The
 * optimizer can be constrained to search for a minimum within a feasible 
 * region. The feasible region can be defined by setting limits on the 
 * paramters of the objective function and/or supplying constraint 
 * functions that must be satisfied. 
 * The optimizer start searching for a minimum begining at a user supplied 
 * initial set of paramters.
 *
 * The objective function and constraints are specified by supplying the
 * Optimizer with an implemenation of the OptimizerSystem class.
 * The OptimizerSystem can be passed to the Optimizer either through the 
 * Optimizer constructor or by calling the setOptimizerSystem method.  
 * The Optimizer class will select the best optimization algorithm to solve the
 * problem based on the  constraints supplied by the OptimizerSystem. 
 * A user can also override the 
 * optimization algorithm selected by the Optimizer by specifying the 
 * optimization algorithm. 
 *  
 */

class SimTK_SIMMATH_EXPORT Optimizer  {

   public:

    Optimizer();
    Optimizer( const OptimizerSystem& sys);
    Optimizer( const OptimizerSystem& sys, OptimizerAlgorithm algorithm);


    ~Optimizer();

    static bool isAlgorithmAvailable(OptimizerAlgorithm algorithm);
   
    /// sets the absolute tolerance used determine if the problem has converged
    void setConvergenceTolerance( const Real tolerance );
    /// set the maximum number of iterations used for each step  
    void setMaxIterations( const int iter );
    /// set the maximum number of previous hessians used in a limitied memory hessian approximation  
    void setLimitedMemoryHistory( const int history );
    /// set the level of debugging info displayed 
    void setDiagnosticsLevel( const int level ); 
    /// set which numerical gradient algorithm is used 
    void setDifferentiatorMethod( Differentiator::Method method);

    void setOptimizerSystem( const OptimizerSystem& sys  );
    void setOptimizerSystem( const OptimizerSystem& sys, OptimizerAlgorithm algorithm );

    /// set the value of an advanced option specified by a string
    bool setAdvancedStrOption( const char *option, const char *value );
    /// set the value of an advanced option specified by a real value
    bool setAdvancedRealOption( const char *option, const Real value );
    /// set the value of an advanced option specified by an integer value
    bool setAdvancedIntOption( const char *option, const int value );
    /// set the value of an advanced option specified by an boolean value
    bool setAdvancedBoolOption( const char *option, const bool value );

    /// enable numerical gradients 
    void useNumericalGradient( const bool flag );
    /// enable numerical Jacobian 
    void useNumericalJacobian( const bool flag );

    /// compute optimization
    double optimize(Vector&);
    
private:
  typedef int (*ObjectiveFunc)      ( const OptimizerSystem&,
                                    const Vector& parameters,  const bool new_parameters,
                                    Real& f );

  typedef int (*GradientFunc)       ( const OptimizerSystem&,
                                    const Vector &parameters, const bool new_parameters,
                                    Vector &gradient );

  typedef int (*ConstraintFunc)     ( const OptimizerSystem&,
                                    const Vector & parameters, const bool new_parameters,
                                    Vector &constraints );

  typedef int (*ConstraintJacobian) ( const OptimizerSystem&,
                                    const Vector& parameters, const bool new_parameters,
                                    Matrix& jac );

  typedef int (*Hessian)            ( const OptimizerSystem&,
                                    const Vector &parameters, const bool new_parameters,
                                    Vector &gradient);

  void registerObjectiveFunc( ObjectiveFunc );
  void registerGradientFunc( GradientFunc );
  void registerConstraintFunc( ConstraintFunc );
  void registerConstraintJacobian( ConstraintJacobian );
  void registerHessian( Hessian );

    // This is the library-side part of the CPodes constructor. This must
    // be done prior to the client side construction.
  void librarySideOptimizerConstructor(const OptimizerSystem& sys, OptimizerAlgorithm algorithm);

  void clientSideOptimizerConstructor() {
       registerObjectiveFunc( objectiveFunc_static );
       registerGradientFunc( gradientFunc_static );
       registerConstraintFunc( constraintFunc_static );
       registerConstraintJacobian( constraintJacobian_static );
       registerHessian( hessian_static );
  }

    class OptimizerRep* rep;
    friend class OptimizerRep;

    const OptimizerRep& getRep() const {assert(rep); return *rep;}
    OptimizerRep&       updRep()       {assert(rep); return *rep;}

    Optimizer( const Optimizer& c );
    Optimizer& operator=(const Optimizer& rhs);

}; // Class Optimizer
 
} // namespace SimTK

#endif //_SimTK_OPTIMIZER_H
