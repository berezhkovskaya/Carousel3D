#include "Animation.h"
#include "Environment.h"
#include "Core/Linker.h"
#include "Core/Console.h"
#include "Resources/ResourcesManager.h"

void clAnimSet::AddAnimSequence( const sAnimSequence& Anim )
{
	FAnimations.FStream.push_back( Anim );
}

sAnimSequence clAnimSet::GetAnimSequenceByName( const LString& Name ) const
{
	for ( size_t i = 0; i != FAnimations.size(); i++ )
	{
		if ( FAnimations[i].FName.ToString() == Name ) { return FAnimations[i]; }
	}

	return sAnimSequence();
}

sAnimSequence clAnimSet::GetAnimSequence( size_t i ) const
{
	return FAnimations[i];
}

LString clAnimSet::GetAnimSequenceName( size_t i ) const
{
	return FAnimations[i].FName.ToString();
}

LVector4 clAnimSet::GetAnimSequenceParams( size_t i ) const
{
	return FAnimations[i].ToVector4();
}

size_t clAnimSet::GetNumAnimSequences() const
{
	return FAnimations.size();
}

void clAnimSet::LoadAnimSet( const LString& FileName )
{
	guard( "%s", FileName.c_str() );

	FAnimations.FStream.clear();

	iIStream* IStream = Env->FileSystem->CreateFileReader( FileName );

	while ( !IStream->Eof() )
	{
		LString Line = IStream->ReadLine();

		LString Name      = LStr::GetToken( Line, 1 );
		int FirstFrame    = LStr::ToInt( LStr::GetToken( Line, 2 ) );
		// The third token is NOT the number of frames, it is the LastFrame
		int Frames        = LStr::ToInt( LStr::GetToken( Line, 3 ) ) - FirstFrame + 1;
		int LoopingFrames = LStr::ToInt( LStr::GetToken( Line, 4 ) );
		int FPS           = LStr::ToInt( LStr::GetToken( Line, 5 ) );

		if ( Frames == 0        ) { continue; }

		if ( LoopingFrames == 0 ) { LoopingFrames = Frames; }

		if ( FPS == 0           ) { FPS = 20; }

		sAnimSequence Seq( Name, FirstFrame, Frames, LoopingFrames, FPS );

		AddAnimSequence( Seq );
	}

	delete( IStream );

	unguard();
}

clAnimation::clAnimation()
	: FDefaultAnimSet( NULL ),
	  FCurrentAnimSet( NULL )
{
}

clAnimation::~clAnimation()
{
	if ( FCurrentAnimSet != FDefaultAnimSet ) { delete( FCurrentAnimSet ); }

	delete( FDefaultAnimSet );
}

void clAnimation::AfterConstruction()
{
	iResource::AfterConstruction();

	FDefaultAnimSet = Env->Linker->Instantiate( "clAnimSet" );
	FDefaultAnimSet->SetObjectID( "FDefaultAnimSet" );

	FCurrentAnimSet = FDefaultAnimSet;
}

LString clAnimation::GetCachingDir() const
{
	return Env->Console->GetVarValueStr( "Cache.AnimationsCachingDir", "Cache/Animations" );
}

void clAnimation::CacheTo( const LString& FileName )
{
}

bool clAnimation::CacheFrom( const LString& FileName )
{
	return false;
}

void clAnimation::SetAnimSet( clAnimSet* AnimSet )
{
	LMutex Lock( &FMutex );

	FCurrentAnimSet = AnimSet;

	SendAsync( L_EVENT_CHANGED, LEventArgs(), false );
}

clAnimSet* clAnimation::GetCurrentAnimSet() const
{
	LMutex Lock( &FMutex );

	return FCurrentAnimSet;
}

clAnimSet* clAnimation::GetDefaultAnimSet() const
{
	LMutex Lock( &FMutex );

	return FDefaultAnimSet;
}

clAnimation* clAnimation::CloneAnimation() const
{
	clAnimation* TheCopy = Env->Resources->CreateAnimation();

//	TheCopy->SetAnimSet( FCurrentAnimSet->MakeCopy() );

	return TheCopy;
}

bool clAnimation::LoadFromFile( const LString& FileName )
{
	guard( "%s", FileName.c_str() );

	GetCurrentAnimSet()->LoadAnimSet( FileName );

	return true;

	unguard();
}

/*
 * 22/09/2011
     It's here
*/
