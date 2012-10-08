/**
 * \file VisualGraph.h
 * \brief Visual graph
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __VisualGraph__h__included__
#define __VisualGraph__h__included__

#include "VisualRenderables.h"

#include "Math/LGraph.h"

class clGeom;
class clMaterial;

/**
   Visual representation of a graph for debugging purposes.

   Specific graphs may also be used in some logical items, like waypoint/accessibility maps
*/
class scriptfinal netexportable clVisualGraph : public clVisualRenderable
{
public:
	clVisualGraph();
	virtual ~clVisualGraph();

	virtual void AddDebugGeometry( clScene* S );
	virtual void RemoveDebugGeometry( clScene* S );

	SERIALIZABLE_CLASS()

	///  TODO : move to VisualObject ?
	virtual void RefreshDebugGeometry( clScene* S )
	{
		RemoveDebugGeometry( S );

		/// do anything to rebuild current visual geometry

		AddDebugGeometry( S );
	}

	virtual void UpdateTransform();

	scriptmethod clGraph*  GetGraph() const { return FGraph; }
	scriptmethod void      SetGraph( clGraph* GG ) { FGraph = GG; }
public:
	/* Property(Name=Graph, Type=clGraph, FieldName=FGraph) */

	clGraph* FGraph;

	/* Property(Name=BeadRadius,  Type=float, FieldName=FBeadRadius)  */
	/* Property(Name=StickRadius, Type=float, FieldName=FStickRadius) */
	/* Property(Name=ArrowSize,   Type=float, FieldName=FArrowSize)   */
	/* Property(Name=UseCubes,    Type=bool,  FieldName=FUseCubes)    */

	/// TODO : what to do with Value/ValueFileName pairs in serialization ?
	/// Write specific getters/setters, which resolve the conflits or something else ?

	/* Property(Name=BeadMaterial,  Type=string, Setter=SetBeadMaterialName,  Getter=GetBeadMaterialName)  */
	/* Property(Name=StickMaterial, Type=string, Setter=SetStickMaterialName, Getter=GetStickMaterialName) */
	/* Property(Name=ArrowMaterial, Type=string, Setter=SetArrowMaterialName, Getter=GetArrowMaterialName) */

	/// Set the name of material
	void SetMaterialName( const LString& Name, clMaterial** MatPtr );
	LString GetMaterialName( clMaterial* MatPtr ) const;

	scriptmethod void    SetBeadMaterialName ( const LString& Name ) { SetMaterialName( Name, &FBeadMaterial );  }
	scriptmethod void    SetStickMaterialName( const LString& Name ) { SetMaterialName( Name, &FStickMaterial ); }
	scriptmethod void    SetArrowMaterialName( const LString& Name ) { SetMaterialName( Name, &FArrowMaterial ); }

	scriptmethod LString GetBeadMaterialName()  const { return GetMaterialName( FBeadMaterial );  }
	scriptmethod LString GetStickMaterialName() const { return GetMaterialName( FStickMaterial ); }
	scriptmethod LString GetArrowMaterialName() const { return GetMaterialName( FArrowMaterial ); }

#pragma region Visual properties

	float FBeadRadius;
	float FStickRadius;
	float FArrowSize;

	bool  FUseCubes;

	clMaterial* FBeadMaterial;
	clMaterial* FStickMaterial;
	clMaterial* FArrowMaterial;

#pragma endregion

private:
	/// Get current graph mesh
	clGeom* GetGraphGeometry();

	/// Internal container of graph mesh
	clGeom* FGraphGeometry;

	/// The internal ID of graph debug geometry
	int FGraphGeomID;
};

#endif

/*
 * 09/12/2010
     Log section added
*/
