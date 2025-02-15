//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/script.h                                      $
// $Revision:: 8                                                              $
//   $Author:: Steven                                                         $
//     $Date:: 10/13/03 8:54a                                                 $
//
// Copyright (C) 1997 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// C++ implementaion of tokenizing text interpretation.  Class accepts filename
// to load or pointer to preloaded text data.  Standard tokenizing operations
// such as skip white-space, get string, get integer, get float, get token,
// and skip line are implemented.
//
// Note: all '//', '#', and ';' are treated as comments.  Probably should
// make this behaviour toggleable.
//

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include "class.h"
#include "vector.h"
#include "utils/str.h"

#define	MAXTOKEN	256

struct scriptmarker_s
{
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	qboolean		tokenready = qfalse;
	int         offset = 0;
	int			line = 0;
	char			token[MAXTOKEN] = {0};
};
using scriptmarker_t = scriptmarker_s;

typedef struct
	{
	//const char		*macroName;
	//const char		*macroText;
	str				macroName;
	str				macroText;
	} macro;

class Script : public Class
	{
	protected:
		str			filename;
		const char* script_p;
		const char* end_p;
		Container<macro*> macrolist;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean		tokenready = qfalse;
		int				line = 0;
		char			token[MAXTOKEN] = {0};
		qboolean		releaseBuffer = qfalse;


		qboolean		AtComment( void );
		void			CheckOverflow( void );

	public:
		const char	*buffer;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int			length;


      CLASS_PROTOTYPE( Script );

		~Script();
		Script(const char* filename = 0);
		void			Close( void );
		const char *Filename( void );
		int			GetLineNumber( void );
		void			Reset( void );
		void			MarkPosition( scriptmarker_t *mark );
		void			RestorePosition( const scriptmarker_t *mark );
		qboolean		SkipToEOL( void );
		void			SkipWhiteSpace( qboolean crossline );
		void			SkipNonToken( qboolean crossline );
		qboolean		TokenAvailable( qboolean crossline );
		qboolean		CommentAvailable( qboolean crossline );
		void			UnGetToken( void );
		qboolean		AtString( qboolean crossline );
		qboolean		AtOpenParen( qboolean crossline );
		qboolean		AtCloseParen( qboolean crossline );
		qboolean		AtComma( qboolean crossline );
		const char		*GetToken( qboolean crossline );
		const char		*GetLine( qboolean crossline );
		const char		*GetRaw( void );
		const char		*GetString( qboolean crossline );
		qboolean		GetSpecific( const char *string );
		qboolean		GetBoolean( qboolean crossline );
		int				GetInteger( qboolean crossline );
		double			GetDouble( qboolean crossline );
		float			GetFloat( qboolean crossline );
		Vector			GetVector( qboolean crossline );
		int				LinesInFile( void );
		void			Parse( const char *data, int length, const char *name );
		void			LoadFile( const char *name );
		const char		*Token( void );
		virtual void	Archive( Archiver &arc );
		void			AddMacroDefinition( qboolean crossline );
		const char		*GetMacroString( const char *theMacroName );
		char			*EvaluateMacroString( const char *theMacroString );
		float			EvaluateMacroMath(float value, float newval, char oper);
		const char		*GetExprToken(const char *ptr, char *token);
		const char		*GrabNextToken( qboolean crossline );
		qboolean		isMacro( void );

		Container<macro *>	*GetMacroList() { return &macrolist; }
		void				AddMacro(const char *name, const char *value);
	};

inline void Script::Archive
	(
	Archiver &arc
	)
   {
   int pos;

   Class::Archive( arc );

   arc.ArchiveBoolean( &tokenready );

   arc.ArchiveString( &filename );
   if ( arc.Loading() )
      {
      //
      // load the file in
      //
      LoadFile( filename.c_str() );
      }

   if ( arc.Saving() )
      {
      //
      // save out current pointer as an offset
      //
      pos = script_p - buffer;
      }
   arc.ArchiveInteger( &pos );
   if ( arc.Loading() )
      {
      //
      // restore the script pointer
      //
      script_p = buffer + pos;
      }

   	//const char	*end_p;
	//Container<macro *> macrolist;

   arc.ArchiveInteger( &line );
   arc.ArchiveRaw( token, sizeof( token ) );

   //qboolean		releaseBuffer;

   }

#endif
