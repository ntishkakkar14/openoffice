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



// used interfaces
import com.sun.star.uno.XComponentContext;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.linguistic2.XLinguServiceManager;
import com.sun.star.linguistic2.XSpellChecker;
import com.sun.star.linguistic2.XHyphenator;
import com.sun.star.linguistic2.XThesaurus;
import com.sun.star.linguistic2.XSpellAlternatives;
import com.sun.star.linguistic2.XHyphenatedWord;
import com.sun.star.linguistic2.XPossibleHyphens;
import com.sun.star.linguistic2.XMeaning;
import com.sun.star.linguistic2.XSearchableDictionaryList;
import com.sun.star.linguistic2.XLinguServiceEventListener;
import com.sun.star.linguistic2.LinguServiceEvent;
import com.sun.star.beans.XPropertySet;
import com.sun.star.beans.PropertyValue;
import com.sun.star.uno.XComponentContext;
import com.sun.star.uno.XNamingService;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.EventObject;
import com.sun.star.lang.Locale;
import com.sun.star.bridge.XUnoUrlResolver;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.Any;
import com.sun.star.lang.XComponent;

public class LinguisticExamples
{
    // The remote office ocntext
    protected XComponentContext mxRemoteContext = null;
    // The MultiServiceFactory interface of the Office
    protected XMultiComponentFactory mxRemoteServiceManager = null;

    // The LinguServiceManager interface
    protected XLinguServiceManager mxLinguSvcMgr = null;

    // The SpellChecker interface
    protected XSpellChecker mxSpell = null;

    // The Hyphenator interface
    protected XHyphenator mxHyph = null;

    // The Thesaurus interface
    protected XThesaurus  mxThes = null;

    // The DictionaryList interface
    protected XSearchableDictionaryList  mxDicList = null;

    // The LinguProperties interface
    protected XPropertySet  mxLinguProps = null;


    public static void main(String args[]) 
	{
		// Create an instance of the class and call it's begin method
        try {
            LinguisticExamples aExample = new LinguisticExamples();
            aExample.Connect();
            aExample.Run();        
        } catch (Exception e) { 
            System.err.println("failed to run examples");
            e.printStackTrace();
        }
	}


    public void Connect()
        throws java.lang.Exception
    {
        // get the remote office context. If necessary a new office
        // process is started
        mxRemoteContext = com.sun.star.comp.helper.Bootstrap.bootstrap();
        System.out.println("Connected to a running office ...");
        mxRemoteServiceManager = mxRemoteContext.getServiceManager();
    }


    /** Get the LinguServiceManager to be used. For example to access spell
        checker, thesaurus and hyphenator, also the component may choose to
        register itself as listener to it in order to get notified of relevant
        events. */
    public boolean GetLinguSvcMgr()
        throws com.sun.star.uno.Exception
    {
        if (mxRemoteContext != null && mxRemoteServiceManager != null) {
            Object aObj = mxRemoteServiceManager.createInstanceWithContext(
                "com.sun.star.linguistic2.LinguServiceManager", mxRemoteContext );
            mxLinguSvcMgr = (XLinguServiceManager) 
                    UnoRuntime.queryInterface(XLinguServiceManager.class, aObj);
        }
        return mxLinguSvcMgr != null;
    }    
    

    /** Get the SpellChecker to be used.
    */
    public boolean GetSpell()
        throws com.sun.star.uno.Exception,
        com.sun.star.uno.RuntimeException
    {
        if (mxLinguSvcMgr != null)
            mxSpell = mxLinguSvcMgr.getSpellChecker();
        return mxSpell != null;
    }

    /** Get the Hyphenator to be used.
    */
    public boolean GetHyph()
        throws com.sun.star.uno.Exception,
        com.sun.star.uno.RuntimeException
    {
        if (mxLinguSvcMgr != null)
            mxHyph = mxLinguSvcMgr.getHyphenator();
        return mxHyph != null;
    }

    /** Get the Thesaurus to be used.
    */
    public boolean GetThes()
        throws com.sun.star.uno.Exception,
        com.sun.star.uno.RuntimeException
    {
        if (mxLinguSvcMgr != null)
            mxThes = mxLinguSvcMgr.getThesaurus();
        return mxThes != null;
    }


    public void Run()
        throws Exception
    {
        GetLinguSvcMgr();


        // list of property values to used in function calls below.
        // Only properties with values different from the (default) values
        // in the LinguProperties property set need to be supllied.
        // Thus we may stay with an empty list in order to use the ones
        // form the property set.
        PropertyValue[] aEmptyProps = new PropertyValue[0];

        // use american english as language
        Locale aLocale = new Locale("en","US","");
    
        
        
        // another list of property values to used in function calls below.
        // Only properties with values different from the (default) values
        // in the LinguProperties property set need to be supllied.
        PropertyValue[] aProps = new PropertyValue[1];
        aProps[0] = new PropertyValue();
        aProps[0].Name  = "IsGermanPreReform";
        aProps[0].Value = new Boolean( true );
    
    
        GetSpell();    
        if (mxSpell !=  null)
        {
            // test with correct word
            String aWord = "horseback";
            boolean bIsCorrect = mxSpell.isValid( aWord, aLocale, aEmptyProps );
            System.out.println( aWord + ": " +  bIsCorrect );

            // test with incorrect word
            aWord = "course";
            bIsCorrect = mxSpell.isValid( aWord, aLocale , aEmptyProps );
            System.out.println( aWord + ": " +  bIsCorrect );    


            aWord = "house";
            XSpellAlternatives xAlt = mxSpell.spell( aWord, aLocale, aEmptyProps );
            if (xAlt == null)
                System.out.println( aWord + " is correct." );
            else
            {
                System.out.println( aWord + " is not correct. A list of proposals follows." );
                String[] aAlternatives = xAlt.getAlternatives();
                if (aAlternatives.length == 0)
                    System.out.println( "no proposal found." );
                else
                {
                    for (int i = 0; i < aAlternatives.length; ++i)
                        System.out.println( aAlternatives[i] );
                }
            }
        }


        GetHyph();    
        if (mxHyph != null)
        {
            // maximum number of characters to remain before the hyphen 
            // character in the resulting word of the hyphenation
            short nMaxLeading = 6;

            XHyphenatedWord xHyphWord = mxHyph.hyphenate( "waterfall",
                                                          aLocale, nMaxLeading ,
                                                          aEmptyProps );
            if (xHyphWord == null)
                System.out.println( "no valid hyphenation position found" );
            else
            {
                System.out.println( "valid hyphenation pos found at "
                                    + xHyphWord.getHyphenationPos() 
                        + " in " + xHyphWord.getWord() );
                System.out.println( "hyphenation char will be after char "
                                    + xHyphWord.getHyphenPos() 
                        + " in " + xHyphWord.getHyphenatedWord() );
            }


            //! Note: 'aProps' needs to have set 'IsGermanPreReform' to true!
            xHyphWord = mxHyph.queryAlternativeSpelling( "Schiffahrt", 
                                new Locale("de","DE",""), (short)4, aProps );
            if (xHyphWord == null)
                System.out.println( "no alternative spelling found at specified position." );
            else
            {
                if (xHyphWord.isAlternativeSpelling())
                    System.out.println( "alternative spelling detectetd!" );
                System.out.println( "valid hyphenation pos found at "
                                    + xHyphWord.getHyphenationPos() 
                        + " in " + xHyphWord.getWord() );
                System.out.println( "hyphenation char will be after char "
                                    + xHyphWord.getHyphenPos() 
                        + " in " + xHyphWord.getHyphenatedWord() );
            }


            XPossibleHyphens xPossHyph = mxHyph.createPossibleHyphens("waterfall",
                                                                      aLocale,
                                                                      aEmptyProps );
            if (xPossHyph == null)
                System.out.println( "no hyphenation positions found." );
            else
                System.out.println( xPossHyph.getPossibleHyphens() );    
        }    
    
        
        GetThes();    
        if (mxThes != null)    
        {
            XMeaning[] xMeanings = mxThes.queryMeanings("house", aLocale,
                                                        aEmptyProps );
            if (xMeanings == null)
                System.out.println( "nothing found." );
            else
            {
                for (int i = 0; i < xMeanings.length; ++i)
                {
                    System.out.println( "Meaning: " + xMeanings[i].getMeaning() );
                    String[] aSynonyms = xMeanings[i].querySynonyms();
                    for (int k = 0; k < aSynonyms.length; ++k)
                        System.out.println( "    Synonym: " + aSynonyms[k] );
                }
            }    
        }    
        
        
        
        XLinguServiceEventListener aClient = new Client();

        // get access to LinguProperties property set
        Object aObj = mxRemoteServiceManager.createInstanceWithContext(
            "com.sun.star.linguistic2.LinguProperties", mxRemoteContext);
        XPropertySet aLinguProps = (XPropertySet) UnoRuntime.queryInterface(
            XPropertySet.class,aObj);
        
        // set a spellchecker and hyphenator property value to a defined state
        try {
            aLinguProps.setPropertyValue("IsGermanPreReform", new Boolean(true));
        } catch (Exception e) {
        }        

        // now add the client as listener to the service manager to
        // get informed when spellchecking or hyphenation may produce
        // different results then before.
        mxLinguSvcMgr.addLinguServiceManagerListener(aClient);
        
        // change that property value in order to trigger a property change
        // event that eventually results in the listeners
        // 'processLinguServiceEvent' function being called
        try {
            aLinguProps.setPropertyValue("IsGermanPreReform", new Boolean(false));
        } catch (Exception e) {
        }
        
        //! keep the listener and the program alive until the event will
        //! be launched. 
        //! There is a voluntary delay before launching the event!
        // Of course this code would usually not be in a *real* client
        // its
        synchronized(this) {
            try {
                this.wait(4000);
            } catch(Exception e) {
            
            }
        }
        
        //! remove listener before programm termination.
        //! should not be omitted.
        mxLinguSvcMgr.removeLinguServiceManagerListener(aClient);
        
        
        System.exit(0);
    }

    /** simple sample implementation of a clients XLinguServiceEventListener
     *  interface implementation
     */
    public class Client 
            implements XLinguServiceEventListener
    {
        public void disposing ( EventObject aEventObj )
        {
            //! any references to the EventObjects source have to be
            //! released here now!
            
            System.out.println("object listened to will be disposed");
        }

        public void processLinguServiceEvent( LinguServiceEvent aServiceEvent )
        {   
            //! do here whatever you think needs to be done depending
            //! on the event recieved (e.g. trigger background spellchecking
            //! or hyphenation again.)
                    
            System.out.println("Listener called");
        }
    };

}

