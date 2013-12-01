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




#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_SHAPE_HXX
#define _SVX_ACCESSIBILITY_ACCESSIBLE_SHAPE_HXX

#include <editeng/AccessibleContextBase.hxx>
#include <editeng/AccessibleComponentBase.hxx>
#include <svx/IAccessibleViewForwarderListener.hxx>
#include <com/sun/star/document/XEventListener.hpp>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleExtendedComponent.hpp>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <svx/AccessibleTextHelper.hxx>
#include "svx/svxdllapi.h"
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLESELECTION_HPP_ 
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLETEXT_HPP_
#include <com/sun/star/accessibility/XAccessibleText.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XAccessibleExtendedAttributes_HPP_
#include <com/sun/star/accessibility/XAccessibleExtendedAttributes.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEGROUPPOSITION_HPP_
#include <com/sun/star/accessibility/XAccessibleGroupPosition.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEHYPERTEXT_HPP_
#include <com/sun/star/accessibility/XAccessibleHypertext.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEHYPERTEXT_HPP_
#include <com/sun/star/accessibility/XAccessibleHypertext.hpp>
#endif
#ifndef INCLUDED_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEHYPERLINK_HPP
#include <com/sun/star/accessibility/XAccessibleHyperlink.hpp>
#endif

#include "ChildrenManager.hxx"

class SdrObject;

namespace accessibility {

class AccessibleShapeInfo;
class AccessibleShapeTreeInfo;
class IAccessibleParent;

/**	This base class provides a base implementation for all shapes.  For more
    detailed documentation about the methods refer to the descriptions of
    the implemented interfaces.  These are, among others,
    <type>XAccessible</type>, <type>XAccessibleContext</type>,
    <type>XAccessibleComponent</type> and
    <type>XAccessibleExtendedComponent</type>.

    <p>The children of a shape can stem from two sources which, in case of
    SVX and SD shapes, are mutually exclusive.  This implementation,
    however, handles both simultaniously to cope with future extensions or
    shapes from other projects.
    <ul>
        <li>If this shape is a group shape, i.e. a
        <type>SvxShapeGroup</type> or a <type>Svx3DSceneObject</type>, it
        can have nested shapes.</li>
        <li>If this shape is a descendant from <type>SvxShapeText</type>
        then the text paragraphs are its children.</li>
    </ul>
    </p>

    <p>Accessible shapes do not listen for disposing() calls of the UNO
    shapes they make accessible.  This is the task of their owner, usually a
    container, who can then call dispose() at the accessible object.</p>
*/
class SVX_DLLPUBLIC AccessibleShape
    :	public AccessibleContextBase,
        public AccessibleComponentBase,
        public ::com::sun::star::accessibility::XAccessibleSelection, 
        public ::com::sun::star::accessibility::XAccessibleExtendedAttributes,
        public ::com::sun::star::accessibility::XAccessibleGroupPosition,
        public com::sun::star::accessibility::XAccessibleHypertext,
        public IAccessibleViewForwarderListener,
        public ::com::sun::star::document::XEventListener,
        public ::com::sun::star::lang::XUnoTunnel
{
public:
	//=====  internal  ========================================================

    /** Create a new accessible object that makes the given shape accessible.
        @param rShapeInfo
            This object contains all information specific to the new
            accessible shape.  That are e.g. the shape to be made accessible
            and the accessible object that will become the parent of the new
            object.
        @param rShapeTreeInfo
            Bundel of information passed to this shape and all of its desendants.
        @attention
            Always call the <member>init</member> method after creating a
            new accessible shape.  This is one way to overcome the potential
            problem of registering the new object with e.g. event
            broadcasters.  That would delete the new object if a broadcaster
            would not keep a strong reference to the new object.
    */
	AccessibleShape (
        const AccessibleShapeInfo& rShapeInfo,
        const AccessibleShapeTreeInfo& rShapeTreeInfo);
	AccessibleShape (
        const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape>& rxShape, const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible>& rxParent, const AccessibleShapeTreeInfo& rShapeTreeInfo, sal_Int32 nIndex = -1);
	 //Solution: Overwrite the object's current name.
    virtual ::rtl::OUString SAL_CALL  	getAccessibleName (void) throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL  	getAccessibleDescription() throw (::com::sun::star::uno::RuntimeException);	
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet> SAL_CALL getAccessibleRelationSet (void) throw (::com::sun::star::uno::RuntimeException);
//=====  XAccessibleSelection  ============================================
	
	virtual void SAL_CALL selectAccessibleChild( 
		sal_Int32 nChildIndex ) 
		throw ( ::com::sun::star::lang::IndexOutOfBoundsException, 
		::com::sun::star::uno::RuntimeException );
	
	virtual sal_Bool SAL_CALL isAccessibleChildSelected( 
		sal_Int32 nChildIndex ) 
		throw ( ::com::sun::star::lang::IndexOutOfBoundsException, 
		::com::sun::star::uno::RuntimeException );
	
	virtual void SAL_CALL clearAccessibleSelection(  ) 
		throw ( ::com::sun::star::uno::RuntimeException );
	
	virtual void SAL_CALL selectAllAccessibleChildren(  ) 
		throw ( ::com::sun::star::uno::RuntimeException );
	
	virtual sal_Int32 SAL_CALL getSelectedAccessibleChildCount(  ) 
		throw ( ::com::sun::star::uno::RuntimeException );
	
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getSelectedAccessibleChild( 
		sal_Int32 nSelectedChildIndex ) 
		throw ( ::com::sun::star::lang::IndexOutOfBoundsException, 
		::com::sun::star::uno::RuntimeException);
	
	virtual void SAL_CALL deselectAccessibleChild( 
		sal_Int32 nSelectedChildIndex ) 
		throw ( ::com::sun::star::lang::IndexOutOfBoundsException, 
		::com::sun::star::uno::RuntimeException );

	// ====== XAccessibleExtendedAttributes =====================================
	virtual ::com::sun::star::uno::Any SAL_CALL getExtendedAttributes() 
	throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException) ;
	///	Return this object's role.
	virtual sal_Int16 SAL_CALL getAccessibleRole (void) throw (::com::sun::star::uno::RuntimeException);
    //=====  XAccessibleGroupPosition  =========================================
    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL
        getGroupPosition( const ::com::sun::star::uno::Any& rAny )
        throw (::com::sun::star::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getObjectLink( const ::com::sun::star::uno::Any& accoject )
        throw (::com::sun::star::uno::RuntimeException);
    /** The destructor releases its children manager and text engine if
        still existent.  These are responsible to send appropriate events.
    */
	virtual ~AccessibleShape (void);

    /** Initialize a new shape.  See the documentation of the constructor
        for the reason of this method's existence.
    */
    virtual void Init (void);

    /** Compare two accessible shapes using object identity
        @param rShape
            This is the second operand.
        @return
            Returns true if both shapes are the same object.
    */
    virtual bool operator== (const AccessibleShape& rShape);

    /** Set the specified state.  If the state is <const>FOCUSED</const>
        then, additionally to the inherited functionality, the focus
        listeners registered with the <type>XAccessibleComponent</type>
        interface are called (if that state really changes).

        @param aState
            The state to turn on.

        @return
            The returned flag indicates whether the specified state has been
            changed (<TRUE/>), i.e. it has formerly not been set.
    */
    virtual sal_Bool SetState (sal_Int16 aState);

    /** Reset the specified state.  If the state is <const>FOCUSED</const>
        then, additionally to the inherited functionality, the focus
        listeners registered with the <type>XAccessibleComponent</type>
        interface are called (if that state really changes).

        @param aState
            The state to turn off.

        @return
            The returned flag indicates whether the specified state has been
            changed (<TRUE/>), i.e. it has formerly been set.
    */
    virtual sal_Bool ResetState (sal_Int16 aState);

    /** Return the state of the specified state.  Take the
        <const>FOCUSED</const> state from the accessible edit engine.

        @param aState
            The state for which to return its value.
        @return
            A value of <TRUE/> indicates that the state is set.  A <FALSE/>
            value indicates an unset state or the inability to access the
            entity that manages the state set.
    
    */
    sal_Bool GetState (sal_Int16 aState);


	//=====  XAccessibleContext  ==============================================

    ///	Return the number of currently visible children.
    virtual sal_Int32 SAL_CALL
    	getAccessibleChildCount (void)
        throw (::com::sun::star::uno::RuntimeException);
        
    /**	Return the specified child.
        @param nIndex
            Index of the requested child.
        @return
            Reference of the requested child which is the accessible object
            of a visible shape.
        @raises IndexOutOfBoundsException
            Throws an exception if the index is not valid.
    */
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible> SAL_CALL 
    	getAccessibleChild (sal_Int32 nIndex)
        throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);


    ///	Return the set of current states.
	virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessibleStateSet> SAL_CALL
    	getAccessibleStateSet (void)
        throw (::com::sun::star::uno::RuntimeException);

	///	Return this objects index among the parents children.
	virtual	sal_Int32 SAL_CALL
    	getAccessibleIndexInParent (void)
        throw (::com::sun::star::uno::RuntimeException);

    //=====  XAccessibleComponent  ============================================

    virtual ::com::sun::star::uno::Reference< 
        ::com::sun::star::accessibility::XAccessible > SAL_CALL 
        getAccessibleAtPoint (const ::com::sun::star::awt::Point& aPoint) 
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds (void) 
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Point SAL_CALL getLocation (void) 
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen (void) 
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Size SAL_CALL getSize (void) 
        throw (::com::sun::star::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL getForeground (void) 
        throw (::com::sun::star::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL getBackground (void) 
        throw (::com::sun::star::uno::RuntimeException);

	

    //=====  XComponent  ========================================================

    using WeakComponentImplHelperBase::addEventListener;
    using WeakComponentImplHelperBase::removeEventListener;

    //=====  XAccessibleEventBroadcaster  =====================================

    /** This call is forwarded to a) the base class and b) to the
        accessible edit engine if it is present.

        @param rxListener
            This listener is informed about accessibility events.
    */
    virtual void SAL_CALL 
        addEventListener (
            const ::com::sun::star::uno::Reference< 
            ::com::sun::star::accessibility::XAccessibleEventListener >& rxListener) 
        throw (::com::sun::star::uno::RuntimeException);

    /** This call is forwarded to a) the base class and b) to the
        accessible edit engine if it is present.

        @param rxListener
            This listener will not be informed about accessibility events
            anymore.
    */
    virtual void SAL_CALL 
        removeEventListener ( 
            const ::com::sun::star::uno::Reference< 
            ::com::sun::star::accessibility::XAccessibleEventListener >& rxListener) 
        throw (::com::sun::star::uno::RuntimeException);


    //=====  XInterface  ======================================================
    
    virtual com::sun::star::uno::Any SAL_CALL
        queryInterface (const com::sun::star::uno::Type & rType)
        throw (::com::sun::star::uno::RuntimeException);
        
    virtual void SAL_CALL
        acquire (void) 
        throw ();
        
    virtual void SAL_CALL
        release (void)
        throw ();
        
        
	//=====  XServiceInfo  ====================================================
    
    /**	Returns an identifier for the implementation of this object.
    */
	virtual ::rtl::OUString SAL_CALL
    	getImplementationName (void)
	    throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
        getSupportedServiceNames (void)
        throw (::com::sun::star::uno::RuntimeException);

    //=====  XTypeProvider  ===================================================
    
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> SAL_CALL
        getTypes (void)
        throw (::com::sun::star::uno::RuntimeException);

    //=====  IAccessibleViewForwarderListener  ================================
    virtual void ViewForwarderChanged (ChangeType aChangeType,
        const IAccessibleViewForwarder* pViewForwarder);

    //=====  lang::XEventListener  ============================================

    /** Listen for disposing events of the model.  The accessible shape
        remains functional when this happens.
    */
    virtual void SAL_CALL 
        disposing (const ::com::sun::star::lang::EventObject& Source) 
        throw (::com::sun::star::uno::RuntimeException);

    //=====  document::XEventListener  ========================================

    virtual void SAL_CALL
        notifyEvent (const ::com::sun::star::document::EventObject& rEventObject)
        throw (::com::sun::star::uno::RuntimeException);


    //===== XUnoTunnel ========================================================
	
    static const ::com::sun::star::uno::Sequence< sal_Int8 >&	getUnoTunnelImplementationId() throw();
	static AccessibleShape*										getImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxIFace ) throw();
	sal_Int64													SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& _rIdentifier ) throw(::com::sun::star::uno::RuntimeException);

	//===== XAccessibleHypertext ========================================================
    virtual sal_Int32 SAL_CALL getHyperLinkCount() 	throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleHyperlink > 
		SAL_CALL getHyperLink( sal_Int32 nLinkIndex ) 	
		throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getHyperLinkIndex( sal_Int32 nCharIndex )
		throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
	//=====  XAccesibleText  ==================================================
		virtual sal_Int32 SAL_CALL getCaretPosition(  ) throw (::com::sun::star::uno::RuntimeException);
		virtual sal_Bool SAL_CALL setCaretPosition( sal_Int32 nIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
		virtual sal_Unicode SAL_CALL getCharacter( sal_Int32 nIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);//Shen Zhen Jie changed sal_Unicode to sal_uInt32; change back to sal_Unicode
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getCharacterAttributes( sal_Int32 nIndex, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aRequestedAttributes ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::awt::Rectangle SAL_CALL getCharacterBounds( sal_Int32 nIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
		virtual sal_Int32 SAL_CALL getCharacterCount(  ) throw (::com::sun::star::uno::RuntimeException);
		virtual sal_Int32 SAL_CALL getIndexAtPoint( const ::com::sun::star::awt::Point& aPoint ) throw (::com::sun::star::uno::RuntimeException);
		virtual ::rtl::OUString SAL_CALL getSelectedText(  ) throw (::com::sun::star::uno::RuntimeException);
		virtual sal_Int32 SAL_CALL getSelectionStart(  ) throw (::com::sun::star::uno::RuntimeException);
		virtual sal_Int32 SAL_CALL getSelectionEnd(  ) throw (::com::sun::star::uno::RuntimeException);
		virtual sal_Bool SAL_CALL setSelection( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
		virtual ::rtl::OUString SAL_CALL getText(  ) throw (::com::sun::star::uno::RuntimeException);
		virtual ::rtl::OUString SAL_CALL getTextRange( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextAtIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextBeforeIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextBehindIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
		virtual sal_Bool SAL_CALL copyText( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);

    //===== Misc ========================================================

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        GetXShape();

	/** set the index _nIndex at the accessible shape
		@param	_nIndex
			The new index in parent.
	*/
	inline void setIndexInParent(sal_Int32 _nIndex) { m_nIndexInParent = _nIndex; }

protected:
    /// Children manager. May be empty if there are no children.
    ChildrenManager* mpChildrenManager;

    /// Reference to the actual shape.
    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape> mxShape;

    /** Bundle of information passed to all shapes in a document tree.
    */
    AccessibleShapeTreeInfo maShapeTreeInfo;

    /** Index that is appended to the object's name to disambiguate between
        different names with the otherwise same name.
    */
    long mnIndex;

	/** the index in parent.
	*/
	sal_Int32 m_nIndexInParent;

    /** The accessible text engine.  May be NULL if it can not be created.
    */
    AccessibleTextHelper* mpText;

    /** This object can be used to modify the child list of our parent.
    */
    IAccessibleParent* mpParent;

	/** This object can be removed when we have an extra interface to ask if the shape is selected
	*/
	SdrObject* m_pShape;

    /** This method is called from the component helper base class while
        disposing.
    */
    virtual void SAL_CALL disposing (void);

    /**	Create a base name string that contains the accessible name.
    */
	virtual ::rtl::OUString
    	CreateAccessibleBaseName (void)
        throw (::com::sun::star::uno::RuntimeException);

    /**	Create a unique name string that contains the accessible name.  The
        name consists of the base name and the index.
    */
	virtual ::rtl::OUString
    	CreateAccessibleName (void)
        throw (::com::sun::star::uno::RuntimeException);

    ///	Create a description string that contains the accessible description.
	virtual ::rtl::OUString
    	CreateAccessibleDescription (void)
        throw (::com::sun::star::uno::RuntimeException);
	virtual ::rtl::OUString
       GetFullAccessibleName(AccessibleShape *shape) 
       throw (::com::sun::star::uno::RuntimeException);
    virtual::rtl::OUString GetStyle();
	void UpdateDocumentAllSelState(::com::sun::star::uno::Reference< 
		::com::sun::star::accessibility::XAccessibleStateSet > &xStateSet);
    /** Update the <const>OPAQUE</const> and <const>SELECTED</const> state.
    */
    virtual void UpdateStates (void);

private:
    /** Don't use the default constructor.  Use the public constructor that 
        takes the original shape and the parent as arguments instead.
    */
    SVX_DLLPRIVATE explicit AccessibleShape (void);
    /// Don't use the copy constructor.  Is there any use for it?
    SVX_DLLPRIVATE explicit AccessibleShape (const AccessibleShape&);
    /// Don't use the assignment operator.  Do we need this?
    SVX_DLLPRIVATE AccessibleShape& operator= (const AccessibleShape&);
    //Solution:Old accessible name
    ::rtl::OUString aAccName;

    /** Call this method when the title, name, or description of the mxShape
        member (may) have been changed.
        This method adapts the name and description members of the
        AccessibleContextBase base class.
    */
    void UpdateNameAndDescription (void);
};

} // end of namespace accessibility

#endif
