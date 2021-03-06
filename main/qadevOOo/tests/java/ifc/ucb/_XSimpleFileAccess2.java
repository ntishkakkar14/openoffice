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



package ifc.ucb;

import lib.MultiMethodTest;

import com.sun.star.io.XInputStream;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.ucb.XSimpleFileAccess2;

/**
* Testing <code>com.sun.star.ucb.XSimpleFileAccess2</code>
* interface methods. <p>
* @see com.sun.star.ucb.XSimpleFileAccess2
*/
public class _XSimpleFileAccess2 extends MultiMethodTest {

    public static XSimpleFileAccess2 oObj = null;

    /**
    * Writes <b>XSimpleFileAccess_new.txt</b> to disk, checks
    * if it was successfully created and then deletes it. <p>
    * Has <b> OK </b> status if after method call the file
    * exists and no exceptions were thrown. <p>
    */
    public void _writeFile() {
        boolean result = true;
        try {
            String dirnameTo = util.utils.getOfficeTemp((XMultiServiceFactory)tParam.getMSF()) ;
            String fileURL = dirnameTo + "XSimpleFileAccess_new.txt";
            String dirname = util.utils.getFullTestURL("XSimpleFileAccess");
            String filename = dirname+"XSimpleFileAccess.txt";
            XInputStream iStream = oObj.openFileRead(filename);
            oObj.writeFile(fileURL,iStream);
            shortWait();
            result = oObj.exists(fileURL);
            oObj.kill(fileURL);
            tRes.tested("writeFile()",result);
        }
        catch (com.sun.star.ucb.CommandAbortedException ex) {
            log.println("CommandAbortedException occured while testing "+
                "'writeFile()'");
            ex.printStackTrace(log);
            tRes.tested("writeFile()",false);
        }
        catch (com.sun.star.uno.Exception ex) {
            log.println("Exception occured while testing 'writeFile()'");
            ex.printStackTrace(log);
            tRes.tested("writeFile()",false);
        }

    } //EOF writeFile()

    /**
    * Sleeps for 1 sec. to allow StarOffice to react on <code>
    * reset</code> call.
    */
    private void shortWait() {
        try {
            Thread.sleep(1000) ;
        } catch (InterruptedException e) {
            log.println("While waiting :" + e) ;
        }
    }

}  // finish class _XSimpleFileAccess

