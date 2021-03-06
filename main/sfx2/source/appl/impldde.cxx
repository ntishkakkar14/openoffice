/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sfx2.hxx"

#if defined(WNT)
#include <tools/svwin.h>
#endif

#include "impldde.hxx"

#include <vcl/svapp.hxx>
#include <vcl/fixed.hxx>
#include <vcl/edit.hxx>
#include <vcl/button.hxx>
#include <vcl/msgbox.hxx>
#include <sot/exchange.hxx>
#include <rtl/ustring.hxx>

#include "dde.hrc"
#include <sfx2/lnkbase.hxx>
#include <sfx2/linkmgr.hxx>
#include "sfx2/sfxresid.hxx"

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>

#include <svl/svdde.hxx>
#include <sot/formats.hxx>

#define DDELINK_COLD		0
#define DDELINK_HOT 		1

#define DDELINK_ERROR_APP	1
#define DDELINK_ERROR_DATA	2
#define DDELINK_ERROR_LINK	3

using namespace ::com::sun::star::uno;

namespace sfx2
{

class SvDDELinkEditDialog : public ModalDialog
{
    FixedText aFtDdeApp;
    Edit aEdDdeApp;
    FixedText aFtDdeTopic;
    Edit aEdDdeTopic;
    FixedText aFtDdeItem;
    Edit aEdDdeItem;
    FixedLine aGroupDdeChg;
    OKButton aOKButton1;
    CancelButton aCancelButton1;

    DECL_STATIC_LINK( SvDDELinkEditDialog, EditHdl_Impl, Edit* );
public:
	SvDDELinkEditDialog( Window* pParent, SvBaseLink* );
	String GetCmd() const;
};

SvDDELinkEditDialog::SvDDELinkEditDialog( Window* pParent, SvBaseLink* pLink )
    : ModalDialog( pParent, SfxResId( MD_DDE_LINKEDIT ) ),
    aFtDdeApp( this, SfxResId( FT_DDE_APP ) ),
    aEdDdeApp( this, SfxResId( ED_DDE_APP ) ),
    aFtDdeTopic( this, SfxResId( FT_DDE_TOPIC ) ),
    aEdDdeTopic( this, SfxResId( ED_DDE_TOPIC ) ),
    aFtDdeItem( this, SfxResId( FT_DDE_ITEM ) ),
    aEdDdeItem( this, SfxResId( ED_DDE_ITEM ) ),
    aGroupDdeChg( this, SfxResId( GROUP_DDE_CHG ) ),
    aOKButton1( this, SfxResId( 1 ) ),
    aCancelButton1( this, SfxResId( 1 ) )
{
    FreeResource();

	String sServer, sTopic, sItem;
	pLink->GetLinkManager()->GetDisplayNames( pLink, &sServer, &sTopic, &sItem );

	aEdDdeApp.SetText( sServer );
	aEdDdeTopic.SetText( sTopic );
	aEdDdeItem.SetText( sItem );

	aEdDdeApp.SetModifyHdl( STATIC_LINK( this, SvDDELinkEditDialog, EditHdl_Impl));
	aEdDdeTopic.SetModifyHdl( STATIC_LINK( this, SvDDELinkEditDialog, EditHdl_Impl));
	aEdDdeItem.SetModifyHdl( STATIC_LINK( this, SvDDELinkEditDialog, EditHdl_Impl));

	aOKButton1.Enable( sServer.Len() && sTopic.Len() && sItem.Len() );
}

String SvDDELinkEditDialog::GetCmd() const
{
	String sCmd( aEdDdeApp.GetText() ), sRet;
    ::sfx2::MakeLnkName( sRet, &sCmd, aEdDdeTopic.GetText(), aEdDdeItem.GetText() );
	return sRet;
}

IMPL_STATIC_LINK( SvDDELinkEditDialog, EditHdl_Impl, Edit *, pEdit )
{
    (void)pEdit; // unused variable
    pThis->aOKButton1.Enable( pThis->aEdDdeApp.GetText().Len() &&
							  pThis->aEdDdeTopic.GetText().Len() &&
							  pThis->aEdDdeItem.GetText().Len() );
	return 0;
}

/*  */


SvDDEObject::SvDDEObject()
	: pConnection( 0 ), pLink( 0 ), pRequest( 0 ), pGetData( 0 ), nError( 0 )
{
	SetUpdateTimeout( 100 );
	bWaitForData = sal_False;
}

SvDDEObject::~SvDDEObject()
{
	delete pLink;
	delete pRequest;
	delete pConnection;
}

sal_Bool SvDDEObject::GetData( ::com::sun::star::uno::Any & rData /*out param*/,
							const String & rMimeType,
							sal_Bool bSynchron )
{
	if( !pConnection )
		return sal_False;

	if( pConnection->GetError() )		// dann versuchen wir es nochmal
	{
		String sServer( pConnection->GetServiceName() );
		String sTopic( pConnection->GetTopicName() );

		delete pConnection;
		pConnection = new DdeConnection( sServer, sTopic );
		if( pConnection->GetError() )
			nError = DDELINK_ERROR_APP;
	}

	if( bWaitForData )		// wir sind rekursiv drin, wieder raus
		return sal_False;

	// Verriegeln gegen Reentrance
	bWaitForData = sal_True;

	// falls gedruckt werden soll, warten wir bis die Daten vorhanden sind
	if( bSynchron )
	{
		DdeRequest aReq( *pConnection, sItem, 5000 );
		aReq.SetDataHdl( LINK( this, SvDDEObject, ImplGetDDEData ) );
		aReq.SetFormat( SotExchange::GetFormatIdFromMimeType( rMimeType ));

		pGetData = &rData;

		do {
			aReq.Execute();
		} while( aReq.GetError() && ImplHasOtherFormat( aReq ) );

		if( pConnection->GetError() )
			nError = DDELINK_ERROR_DATA;

		bWaitForData = sal_False;
	}
	else
	{
		// ansonsten wird es asynchron ausgefuehrt
//		if( !pLink || !pLink->IsBusy() )
		{
			if( pRequest )
				delete pRequest;

			pRequest = new DdeRequest( *pConnection, sItem );
			pRequest->SetDataHdl( LINK( this, SvDDEObject, ImplGetDDEData ) );
			pRequest->SetDoneHdl( LINK( this, SvDDEObject, ImplDoneDDEData ) );
			pRequest->SetFormat( SotExchange::GetFormatIdFromMimeType(
									rMimeType ) );
			pRequest->Execute();
		}

		::rtl::OUString aEmptyStr;
		rData <<= aEmptyStr;
	}
	return 0 == pConnection->GetError();
}


sal_Bool SvDDEObject::Connect( SvBaseLink * pSvLink )
{
#if defined(WNT)
	static sal_Bool bInWinExec = sal_False;
#endif
	sal_uInt16 nLinkType = pSvLink->GetUpdateMode();
	if( pConnection )		// Verbindung steht ja schon
	{
		// tja, dann nur noch als Abhaengig eintragen
		AddDataAdvise( pSvLink,
				SotExchange::GetFormatMimeType( pSvLink->GetContentType()),
				LINKUPDATE_ONCALL == nLinkType
						? ADVISEMODE_ONLYONCE
						: 0 );
		AddConnectAdvise( pSvLink );

		return sal_True;
	}

	if( !pSvLink->GetLinkManager() )
		return sal_False;

	String sServer, sTopic;
	pSvLink->GetLinkManager()->GetDisplayNames( pSvLink, &sServer, &sTopic, &sItem );

	if( !sServer.Len() || !sTopic.Len() || !sItem.Len() )
		return sal_False;

	pConnection = new DdeConnection( sServer, sTopic );
	if( pConnection->GetError() )
	{
		// kann man denn das System-Topic ansprechen ?
		// dann ist der Server oben, kennt nur nicht das Topic!
		if( sTopic.EqualsIgnoreCaseAscii( "SYSTEM" ) )
		{
			sal_Bool bSysTopic;
			{
				DdeConnection aTmp( sServer, String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "SYSTEM" ) ) );
				bSysTopic = !aTmp.GetError();
			}

			if( bSysTopic )
			{
				nError = DDELINK_ERROR_DATA;
				return sal_False;
			}
			// ansonsten unter Win/WinNT die Applikation direkt starten
		}

#if defined(WNT)

		// Server nicht da, starten und nochmal versuchen
		if( !bInWinExec )
		{
			ByteString aCmdLine( sServer, RTL_TEXTENCODING_ASCII_US );
			aCmdLine.Append( ".exe " );
			aCmdLine.Append( ByteString( sTopic, RTL_TEXTENCODING_ASCII_US ) );

			if( WinExec( aCmdLine.GetBuffer(), SW_SHOWMINIMIZED ) < 32 )
				nError = DDELINK_ERROR_APP;
			else
			{
				sal_uInt16 i;
				for( i=0; i<5; i++ )
				{
					bInWinExec = sal_True;
					Application::Reschedule();
					bInWinExec = sal_False;

					delete pConnection;
					pConnection = new DdeConnection( sServer, sTopic );
					if( !pConnection->GetError() )
						break;
				}

				if( i == 5 )
				{
					nError = DDELINK_ERROR_APP;
				}
			}
		}
		else
#endif	// WNT
		{
			nError = DDELINK_ERROR_APP;
		}
	}

	if( LINKUPDATE_ALWAYS == nLinkType && !pLink && !pConnection->GetError() )
	{
		// Hot Link einrichten, Daten kommen irgendwann spaeter
		pLink = new DdeHotLink( *pConnection, sItem );
		pLink->SetDataHdl( LINK( this, SvDDEObject, ImplGetDDEData ) );
		pLink->SetDoneHdl( LINK( this, SvDDEObject, ImplDoneDDEData ) );
		pLink->SetFormat( pSvLink->GetContentType() );
		pLink->Execute();
	}

	if( pConnection->GetError() )
		return sal_False;

	AddDataAdvise( pSvLink,
				SotExchange::GetFormatMimeType( pSvLink->GetContentType()),
				LINKUPDATE_ONCALL == nLinkType
						? ADVISEMODE_ONLYONCE
						: 0 );
	AddConnectAdvise( pSvLink );
	SetUpdateTimeout( 0 );
	return sal_True;
}

void SvDDEObject::Edit( Window* pParent, sfx2::SvBaseLink* pBaseLink, const Link& rEndEditHdl )
{
    SvDDELinkEditDialog aDlg( pParent, pBaseLink );
    if ( RET_OK == aDlg.Execute() && rEndEditHdl.IsSet() )
    {
        String sCommand = aDlg.GetCmd();
        rEndEditHdl.Call( &sCommand );
    }
}

sal_Bool SvDDEObject::ImplHasOtherFormat( DdeTransaction& rReq )
{
	sal_uInt16 nFmt = 0;
	switch( rReq.GetFormat() )
	{
	case FORMAT_RTF:
		nFmt = FORMAT_STRING;
		break;

	case SOT_FORMATSTR_ID_HTML_SIMPLE:
	case SOT_FORMATSTR_ID_HTML:
		nFmt = FORMAT_RTF;
		break;

	case FORMAT_GDIMETAFILE:
		nFmt = FORMAT_BITMAP;
		break;

	case SOT_FORMATSTR_ID_SVXB:
		nFmt = FORMAT_GDIMETAFILE;
		break;

	// sonst noch irgendwas ??
	}
	if( nFmt )
		rReq.SetFormat( nFmt );		// damit nochmal versuchen
	return 0 != nFmt;
}

sal_Bool SvDDEObject::IsPending() const
/*	[Beschreibung]

	Die Methode stellt fest, ob aus einem DDE-Object die Daten gelesen
	werden kann.
	Zurueckgegeben wird:
		ERRCODE_NONE 			wenn sie komplett gelesen wurde
		ERRCODE_SO_PENDING		wenn sie noch nicht komplett gelesen wurde
		ERRCODE_SO_FALSE		sonst
*/
{
	return bWaitForData;
}

sal_Bool SvDDEObject::IsDataComplete() const
{
	return bWaitForData;
}

IMPL_LINK( SvDDEObject, ImplGetDDEData, DdeData*, pData )
{
	sal_uIntPtr nFmt = pData->GetFormat();
	switch( nFmt )
	{
	case FORMAT_GDIMETAFILE:
		break;

	case FORMAT_BITMAP:
		break;

	default:
		{
			const sal_Char* p = (sal_Char*)( pData->operator const void*() );
			long nLen = FORMAT_STRING == nFmt ? (p ? strlen( p ) : 0) : (long)*pData;

			Sequence< sal_Int8 > aSeq( (const sal_Int8*)p, nLen );
			if( pGetData )
			{
				*pGetData <<= aSeq; 	// Daten kopieren
				pGetData = 0;			// und den Pointer bei mir zuruecksetzen
			}
			else
			{
				Any aVal;
				aVal <<= aSeq;
				DataChanged( SotExchange::GetFormatMimeType(
												pData->GetFormat() ), aVal );
				bWaitForData = sal_False;
			}
		}
	}

	return 0;
}

IMPL_LINK( SvDDEObject, ImplDoneDDEData, void*, pData )
{
	sal_Bool bValid = (sal_Bool)(sal_uIntPtr)pData;
	if( !bValid && ( pRequest || pLink ))
	{
		DdeTransaction* pReq = 0;
		if( !pLink || ( pLink && pLink->IsBusy() ))
			pReq = pRequest;		// dann kann nur der fertig sein
		else if( pRequest && pRequest->IsBusy() )
			pReq = pLink;			// dann kann nur der fertig sein

		if( pReq )
		{
			if( ImplHasOtherFormat( *pReq ) )
			{
				pReq->Execute();
			}
			else if( pReq == pRequest )
			{
				// das wars dann
				bWaitForData = sal_False;
			}
		}
	}
	else
		// das warten ist beendet
		bWaitForData = sal_False;

	return 0;
}

}
