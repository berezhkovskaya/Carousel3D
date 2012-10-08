/**
 * \file LVLib.cpp
 * \brief Linderdaum Volume Rendering Library - procedural implementation
 * \version 0.5.89a
 * \date 06/05/2009
 * \author Sergey Kosarevsky, 2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Linderdaum.h"
#include "LVLib.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

#define START_MTD() \
   guard( "%i", VolID );\
   if ( !IsValidID( VolID ) ) return false;

#define END_MTD()\
   return true; \
   unguard();

namespace LVLib
{

/// Internal global parameters of LVLib
	struct sVRGlobals
	{
		sVRGlobals(): FInitialized( false ),
			FVolumeDimensionLimit( 512 ),
			FGradientsMode( L_VRGRADIENTS_FORWARDDIFF ),
			FEnableHQDynamicGradients( false ) {};
		//
		bool              FInitialized;
		int               FVolumeDimensionLimit;
		LGradientsMode    FGradientsMode;
		bool              FEnableHQDynamicGradients;
	};

/// Engine entry point
	sEnvironment*               Env;

/// Global volume rendering parameters
	sVRGlobals                  Globals;

/// Active volumes storage
	LArray<clLVLibVolume*> Volumes;
/// Active volumes visibility
	LArray<bool>           VolumesVisible;

/// User input handler
	class clGUIInputHUD: public iGUIView
	{
	public:
		clGUIInputHUD() {};
		virtual ~clGUIInputHUD() {};
	};
	/*
	/// Iterates the collection of volumes for rendering
	   class clOverlayRenderer: public iObject
	   {
	   public:
	      virtual ~clOverlayRenderer()
	      {
	         Env->DisconnectObject( this );
	      }
	      //
	      // iObject interface
	      //
	      virtual void AfterConstruction()
	      {
	         Env->Connect( L_EVENT_DRAWOVERLAY, BIND( &clOverlayRenderer::Event_DRAWOVERLAY ) );
	      }
	      // events
	      FWD_EVENT_HANDLER( Event_DRAWOVERLAY )
	      {
	         // render all volumes
	         for ( size_t i = 0; i != Volumes.size(); ++i )
	         {
	            if ( VolumesVisible[i] && Volumes[i] )
	            {
	               Volumes[i]->Render();
	            }
	         }
	      }
	   };
	*/
	void DrawOverlayCallback( LEvent Event, const LEventArgs& Args )
	{
		// render all volumes
		for ( size_t i = 0; i != Volumes.size(); ++i )
		{
			if ( VolumesVisible[i] && Volumes[i] )
			{
				Volumes[i]->Render();
			}
		}
	}

/// Global pointer to clOverlayRenderer
//	clOverlayRenderer*    OverlayRenderer;

	void InitEngine_Simple( bool CreateNewWindow, void* ExternalWndHandle )
	{
		Env = new sEnvironment;
		Env->Env = Env;

		Env->DeployEnvironment( NULL,
		                        "default.log",
		                        ".",
		                        "../../CommonMedia",
		                        "volume.cfg",
		                        true,
		                        true,
		                        !CreateNewWindow,
		                        ExternalWndHandle );

		InitEngine_External( Env );
	}

	void InitEngine_External( sEnvironment* ExtEnvironment )
	{
		Env = ExtEnvironment;

		Volumes.clear();

//		REGISTER_CLASS( Env->Linker, clOverlayRenderer );
		REGISTER_CLASS( Env->Linker, clGUIInputHUD );

		Env->Console->SendCommand( "Bind VK_ESCAPE Quit" );
		Env->Console->SendCommand( "Bind VK_F3 RecacheNew" );
		Env->Console->SendCommand( "Bind VK_TAB ToggleConsole" );
		Env->Console->SendCommand( "Bind VK_W \"Toggle WireFrame\"" );
		Env->Console->SendCommand( "Bind VK_F \"Toggle ShowFPS\"" );
		Env->Console->SendCommand( "Bind VK_F9 SaveScreenshot" );
		Env->Console->SendCommand( "Clear" );

//   OverlayRenderer = Env.Linker->Instantiate<clOverlayRenderer>( "LVLib::clOverlayRenderer" );

		Env->Connect( L_EVENT_DRAWOVERLAY, Utils::Bind( &DrawOverlayCallback ) );
		//Env->RegisterEventCallback( L_EVENT_DRAWOVERLAY, &DrawOverlayCallback );

		Globals.FInitialized = true;
	}

	void DoneEngine()
	{
		for ( size_t i = 0; i != Volumes.size(); ++i )
		{
			delete( Volumes[i] );
		}

//		delete( OverlayRenderer );

		Env->ShutdownEnvironment();
	}

	void RunApplication()
	{
		guard();

		Env->RunApplication( DEFAULT_CONSOLE_AUTOEXEC );

		unguard();
	}

	void RenderAndUpdate()
	{
		guard();

		Env->GenerateTicks();

		unguard();
	}

	void RenderFrame()
	{
		guard();

		Env->RenderFrame( 0.0f, 1 );

		unguard();
	}

	sEnvironment* GetEnv()
	{
		return Env;
	}

	int GetRecommendedVolumeDimensionLimit()
	{
		return 512;
	}

	int GetRecommendedIterations()
	{
		return 256;
	}

	void SetVolumeDimensionLimit( int Limit )
	{
		Globals.FVolumeDimensionLimit = Limit;
	}

// need to call before attaching scene node to scene graph
	int CreateVolume()
	{
		guard();

		if ( !Globals.FInitialized ) { return -1; }

		clLVLibVolume* Volume = Env->Linker->Instantiate( "clLVLibVolume" );

		Volumes.push_back( Volume );
		VolumesVisible.push_back( true );

		return static_cast<int>( Volumes.size() ) - 1;

		unguard();
	}

	bool IsValidID( int VolID )
	{
		if ( !Globals.FInitialized ) { return false; }

		if ( VolID < 0 || VolID > static_cast<int>( Volumes.size() ) - 1 )
		{
			return false;
		}

		return ( Volumes[ VolID ] != NULL );
	}

	clLVLibVolume* GetVolume( int VolID )
	{
		return IsValidID( VolID ) ? Volumes[ VolID ] : NULL;
	}

	bool SetVolumeVisibility( int VolID, bool Visible )
	{
		guard( "%i,%i", VolID, Visible );

		if ( !IsValidID( VolID ) )
		{
			return false;
		}

		VolumesVisible[VolID] = Visible;
		END_MTD()
	}

	bool DeleteVolume( int VolID )
	{
		START_MTD()
		delete( Volumes[VolID] );

		VolumesVisible[VolID] = false;
		Volumes[VolID] = NULL;
		END_MTD()
	}

	bool AutoLoadVolumeAndGradients( int VolID,
	                                 const LString& FileName,
	                                 int SizeX,
	                                 int SizeY,
	                                 int SizeZ,
	                                 int BitsPerPixel,
	                                 bool Scale,
	                                 bool PrecomputeGradient,
	                                 bool CreateESLMap )
	{
		guard( "%i,%s,%i,%i,%i,%i,%i,%i,%i", VolID, FileName.c_str(), SizeX, SizeY, SizeZ, BitsPerPixel, Scale, PrecomputeGradient, CreateESLMap );

		if ( !IsValidID( VolID ) )
		{
			return false;
		}

		Volumes[VolID]->AutoLoadVolumeAndGradients( FileName, SizeX, SizeY, SizeZ, BitsPerPixel, Scale, PrecomputeGradient, CreateESLMap );
		END_MTD()
	}

	bool SetVolume( int VolID, clImage* Volume )
	{
		guard( "%i,%p", VolID, Volume );

		if ( !IsValidID( VolID ) )
		{
			return false;
		}

		Volumes[VolID]->SetVolume( Volume );
		END_MTD()
	}

	bool SetVolumeTexture( int VolID, iTexture* Volume )
	{
		guard( "%i,%p", VolID, Volume );

		if ( !IsValidID( VolID ) )
		{
			return false;
		}

		Volumes[VolID]->SetVolumeTexture( Volume );
		END_MTD()
	}

	bool SetGradients( int VolID, clImage* Gradients )
	{
		START_MTD()
		Volumes[VolID]->SetGradients( Gradients );
		END_MTD()
	}

	bool SetESLMap( int VolID, clImage* ESLMap )
	{
		START_MTD()
		Volumes[VolID]->SetESLMap( ESLMap );
		END_MTD()
	}

	bool SetProjection( int VolID, const LMatrix4& Projection )
	{
		START_MTD()
		Volumes[VolID]->SetProjection( Projection );
		END_MTD()
	}

	bool SetModelView( int VolID, const LMatrix4& ModelView )
	{
		START_MTD()
		Volumes[VolID]->SetModelView( ModelView );
		END_MTD()
	}

	bool SetRenderOffscreen( int VolID, bool Offscreen )
	{
		START_MTD()
		Volumes[VolID]->SetRenderOffscreen( Offscreen );
		END_MTD()
	}

	bool SetGradientsMode( int VolID, LGradientsMode Mode )
	{
		START_MTD()
		Volumes[VolID]->SetGradientsMode( Mode );
		END_MTD()
	}

	bool SetScattering( int VolID, bool Scattering )
	{
		START_MTD()
		Volumes[VolID]->SetScattering( Scattering );
		END_MTD()
	}

	bool SetIsosurfaceValue( int VolID, const float IsoValue )
	{
		START_MTD()
		Volumes[VolID]->SetIsosurfaceValue( IsoValue );
		END_MTD()
	}

	bool SetIsosurfaceColor( int VolID, const LVector4& RGBColor )
	{
		START_MTD()
		Volumes[VolID]->SetIsosurfaceColor( RGBColor );
		END_MTD()
	}

	bool SetLightPosition( int VolID, const LVector3& LightPos )
	{
		START_MTD()
		Volumes[VolID]->SetLightPosition( LightPos );
		END_MTD()
	}

	bool EnableClipPlane( int VolID, const LVector4& ClippingPlane, const LVector4& ClipColor, float Thickness )
	{
		START_MTD()
		Volumes[VolID]->EnableClipPlane( ClippingPlane, ClipColor, Thickness );
		END_MTD()
	}

	bool SetClipPlaneParams( int VolID,
	                         const LVector4& ClippingPlane,
	                         const LVector4& ClipColor,
	                         float Thickness )
	{
		START_MTD()
		Volumes[VolID]->SetClipPlaneParams( ClippingPlane, ClipColor, Thickness );
		END_MTD()
	}

	bool DisableClipPlane( int VolID )
	{
		START_MTD()
		Volumes[VolID]->DisableClipPlane();
		END_MTD()
	}

	bool SetIsosurfaceMaterial( int VolID, const LVolumeRenderMaterial Material )
	{
		START_MTD()
		Volumes[VolID]->SetIsosurfaceMaterial( Material );
		END_MTD()
	}

	bool SetLowCutoff( int VolID, const float Value )
	{
		START_MTD()
		Volumes[VolID]->SetLowCutoff( Value );
		END_MTD()
	}

	bool SetOpacitySaturation( int VolID, const float Value )
	{
		START_MTD()
		Volumes[VolID]->SetOpacitySaturation( Value );
		END_MTD()
	}

	bool SetIterations( int VolID, const int Iterations )
	{
		START_MTD()
		Volumes[VolID]->SetIterations( Iterations );
		END_MTD()
	}

	bool SetRenderMode( int VolID, const LVolumeRenderMode Mode )
	{
		START_MTD()
		Volumes[VolID]->SetRenderMode( Mode );
		END_MTD()
	}

	bool SetTransferFunction( int VolID, clImage* TF, LTransferFunctionMode TFMode )
	{
		START_MTD()
		Volumes[VolID]->SetTransferFunction( TF, TFMode );
		END_MTD()
	}

	bool Set2DTextureMap( int VolID, clImage* Map, const LVector2& Scale )
	{
		START_MTD()
		Volumes[VolID]->Set2DTextureMap( Map, Scale );
		END_MTD()
	}

} // namespace

#undef START_MTD
#undef END_MTD

/*
 * 17/05/2009
     SetTransferFunction() instead of group of small functions
 * 06/05/2009
     Set2DTextureMap()
 * 01/05/2009
     SetScattering()
 * 12/04/2009
     External initialization added
 * 23/03/2009
     It's here
*/
