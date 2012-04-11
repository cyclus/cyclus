# query.py
import sqlite3
from numpy import zeros

###############################################################################
###############################################################################

class Query(object) :
    """
    A class representing a Query on a GENIUS output database.
    """

    qType = ''
    """
    The Query type. Right now we support 'material', 'cf', 'power'
    """

    qStmt = None
    """
    The SqlStmt representing this Query.
    """

    conn = None
    """
    The connection to the database.
    """

    data = None
    """
    The array that holds that data itself.
    """

    dataAxes = []
    """
    An ordered list of the names of each of the dimenions in the data array.
    """

    dataUnits = []
    """
    An ordered list of the units on each of the dimenions in the data array.
    """

    dataLabels = [0] * 4
    """
    An ordered list of lists of the labels for each discrete item on each axis.
    """

    isExecuted = False
    """
    True if this Query's SQL statement has been executed and the array contains
    real data.
    """

    isPlottable = False
    """
    True if this Query's data is currently plottable, whatever that will eventually
    mean.
    """

    t0 = 0
    """
    The initial time over which this Query is operating.
    """

    tf = 1200
    """
    The final time over which this Query is operating.
    """

    isoToInd = {}
    """
    A mapping of codes to indices for the isotope dimension.
    """

    indToIso = {}
    """
    A mapping of indices to codes for the isotope dimension.
    """

    ax = None
    """
    Stores the axes formed when plotting the data in this Query.
    """

    figure = None
    """
    Stores the figure formed by plotting the data in this Query.
    """
###############################################################################    

    def __init__(self, file, queryType, t0 = 0, tf = 1200) :
        """
        Creates the Query object by connecting to the database, checking that the
        Query type specified is valid, and initializing a partial SQL statment
        querying the correct quantity over the correct time period. Like the
        python range() function, the rule for the time bounds is that the lower
        is in-clusive and the upper, ex-clusive
        """

        # Save the times.
        self.t0 = t0
        self.tf = tf

        # Check type.
        qTypes = ['material']
        if ( queryType in qTypes) :
            self.qType = queryType
        else :
            raise QueryException, "Error: " + queryType +\
                        " is not a recognized Query type at this time."

        # Initialize the SQL.
        if 'material' == queryType :
            self.qStmt = SqlStmt("Transactions.Time, Transactions.senderID, " + \
                    "Transactions.receiverID, IsotopicStates.IsoID, IsotopicStates.value ", \
                    "Transactions, IsotopicStates",  "Transactions.Time >= " + str(t0) + " AND " + \
                    "Transactions.Time < " + str(tf) )

        self.conn = sqlite3.connect(file)

        # Generate isotope maps.
        isos = getIsoList()
        for index, iso in enumerate(isos) :
            self.isoToInd[iso] = index
            self.indToIso[index] = iso

        # Record the labels and the (default) units for the Query.
        if self.qType == 'material' :
            self.dataAxes = ['time', 'from', 'to', 'iso']
            self.dataUnits = ['months', 'agentID', 'agentID', 'tons']

        
###############################################################################

    def collapseIsos(self) :
        """
        Collapses the isotope set for this Query from whatever it currently is to
        a single mass value, eliminating an entire dimension in the array
        representation. This function may only be called after execution.
        """

        if not self.isExecuted :
            raise QueryException, "Error: operations on the isotope dimension can " + \
                    "can be performed only after Query execution."
        
        try :
            isoDim = self.dataAxes.index('iso')
        except ValueError :
            print "Warning: Query data no longer have an 'isotope' dimension."
            return

        self.data = sum(self.data, isoDim)
        self.dataAxes.pop(isoDim)
        self.dataLabels.pop(isoDim)
        self.dataUnits.pop(isoDim)

###############################################################################

    def collapseSenders(self) :
        """
        Collapses the 'from' set for this Query from whatever it currently is
        to a single, summed value, which is to say we sum over and eliminate
        the 'from' dimension. Thus, this function can only be called after
        execution.
        """
        if not self.isExecuted :
            raise QueryException, "Error: operations on the 'from' dimension can " + \
                        "can be performed only after Query execution. If you want to " + \
                        "limit the number of total actors, used the appropriate " + \
                        "collapseActorsTo...() function."

        try :
            fromDim = self.dataAxes.index('from')
        except ValueError :
            print "Warning: Query data no longer have a 'from' dimension."
            return
        
        self.data = sum(self.data, fromDim)
        self.dataAxes.pop(fromDim)
        self.dataLabels.pop(fromDim)
        self.dataUnits.pop(fromDim)

###############################################################################

    def collapseReceivers(self) :
        """
        Collapses the 'to' set for this Query from whatever it currently is
        to a single, summed value, which is to say we sum over and eliminate
        the 'to' dimension. Thus, this function can only be called after
        execution.
        """
        if not self.isExecuted :
            raise QueryException, "Error: operations on the 'to' dimension can " + \
                        "can be performed only after Query execution. If you want to " + \
                        "limit the number of total actors, used the appropriate " + \
                        "collapseActorsTo...() function."

        try :
            toDim = self.dataAxes.index('to')
        except ValueError :
            print "Warning: Query data no longer have a 'to' dimension."
            return
        
        self.data = sum(self.data, toDim)
        self.dataAxes.pop(toDim)
        self.dataLabels.pop(toDim)
        self.dataUnits.pop(toDim)

###############################################################################

    def collapseSendersAndReceivers(self) :
        """
        Performs a signed material flow analysis by subtracting the array summed
        over 'from' dimension from the array summed over the 'to' dimension. The
        resulting array contains signed material transfer information suitable 
        for plots where we want to study where material resided at a particular
        time. This function can only be called after execution.
        """
        if not self.isExecuted :
            raise QueryException, "Error: operations on the 'from' and 'to'" + \
                        "dimensions can " + \
                        "can be performed only after Query execution. If you want to " + \
                        "limit the number of total actors, used the appropriate " + \
                        "collapseActorsTo...() function."
        try :
            fromDim = self.dataAxes.index('from')
            toDim = self.dataAxes.index('to')
        except ValueError :
            print "Warning: Query data no longer have both a 'from' and 'to' dimension."
            return

        self.data = sum(self.data, fromDim) - sum(self.data, toDim)


        self.dataAxes.pop(fromDim)
        oldLabels = self.dataLabels.pop(fromDim)
        oldUnits = self.dataUnits.pop(fromDim)

        self.dataAxes.insert(fromDim, 'thru')
        self.dataLabels.insert(fromDim, oldLabels)
        self.dataUnits.insert(fromDim, oldUnits)

        self.dataAxes.pop(toDim)
        self.dataLabels.pop(toDim)
        self.dataUnits.pop(toDim)

###############################################################################

    def integrateOverTime(self) :
        """
        Integrates the results of this Query over the time dimension (like
        turning a PDF into a CDF). Especially appropriate for creating
        river plots. This function must be called after execution.
        """

        if not self.isExecuted :
            raise QueryException, "Error: integration over time can only be " + \
                        "performed after Query execution."

        try :
            timeDim = self.dataAxes.index('time')
        except ValueError :
            print "Warning: Query data no longer have both a 'time' dimension."
            return

        self.data = cumsum(self.data, timeDim)
            

###############################################################################
        
    def getData(self) :
        """
        Executes the current form of the Query if it has not been executed already
        and returns the numpy array that stores the data.
        """

        # If we haven't executed, execute.
        if not self.isExecuted :
            self.execute()

        # Return the array.
        return self.data

###############################################################################

    def execute(self) :
        """
        Executes the current form of the Query, storing the data in a numpy array.
        If you want to execute AND return the data, you can use getData().
        """

        # If we've already executed, report as such with an Exception. This may
        # be an important warning. Perhaps we should implement a reExecute for
        # cases where we want the Query to read from the database again rather than
        # simply create a new Query object.
        if self.isExecuted :
            raise QueryException, "Error: This query has already been executed. " + \
                        "Try reExecute()."

        if 'material' == self.qType :

            # Get the array dimensions. We don't know if we've filtered or collapsed
            # away some of
            # the potential result space, so we need to assume the array has the
            # following dimensions (and size).

            # time (tf - t0) X from (numFrom) X to (numTo) X iso (numIsos)
            # (will add commod to this formulation at some point)

            # Count and record how many actors exist during the range of the
            # calculation.
            c = self.conn.cursor()

            actList = []
            c.execute("SELECT Agents.ID FROM Agents, Transactions " + \
                    "WHERE Agents.EnterDate + Agents.LeaveDate > ? " + \
                    "AND Agents.EnterDate <= ? AND " + \
                    "(Agents.ID = Transactions.SenderID OR " + \
                    "Agents.ID = Transactions.ReceiverID) ", (self.t0, self.tf))

            for row in c :
                if row[0] not in actList :
                    actList.append(row[0])

            numActs = len(actList)
            actList.sort()
            
            # Get the list of isotopes from the hard-coded list in getIstList. Count
            # them up and make a dictionary for mapping them into the iso dimension
            # of the Query's data array.
            numIsos = len(self.indToIso)

            # Initialize the array.
            try :
                self.data = zeros( (self.tf - self.t0, numActs, numActs, numIsos) )
            except ValueError :
                raise QueryException, "Error: you've executed a Query whose array " + \
                            "representation would be " + str(self.tf - self.t0) + " x " + \
                            str(numActs) + " x " + str(numActs) + " x " + str(numIsos) + \
                            ". That's too large."

            # Perform the SQL query.
            c.execute(str(self.qStmt))

            # Load the results into the array.
            fromInd = -1
            toInd = -1
            for row in c :
                time = row[0] - self.t0
                fFac = row[1]
                tFac = row[2]

                # Get the indexes for the 'from' and 'to' dimensions.
                d = self.conn.cursor()
                d.execute("SELECT Agents.ID FROM Agents WHERE Agents.ID = ? ", (fFac,))
                        
                for roe in d :
                    fromInd = actList.index(roe[0])

                d.execute("SELECT Agents.ID FROM Agents " + \
                        "WHERE Agents.ID = ? ", (tFac,))
                        
                for roe in d :
                    toInd = actList.index(roe[0])

                # Get the composition.
                comp = convNdToTons(stripLightIsos(makeCompDict(Buffer.read(row[3]))))

                for i, m in comp.iteritems() :
                    self.data[time][fromInd][toInd][self.isoToInd[i]] += m

            # Store the labels.
            self.dataLabels[0] = range(self.t0, self.tf)
            self.dataLabels[1] = actList
            self.dataLabels[2] = actList
            self.dataLabels[3] = self.indToIso.values()
            
        self.isExecuted = True


###############################################################################

    def riverPlot(self, streamDim = None, streamList = None, \
                                selectDim = None, selectItem = None) :
        """
        Creates a river plot of the data in this Query. 'time' and at least one
        (but no more than two) other axes must exist. 

        Plots the data in the dimension 'streamDim' against time. If a
        streamList of label names of items in the streamDim axis is given, we plot
        only those streams instead of all of them. If the data array is currently
        three dimensional, the user must also specify a dimension, selectDim,
        and a label of an element in that dimension, selectItem, to plot.

        3D example:

        To make an isotope-wise river plot for facility 5 when the array
        looks like this:

        dataAxes = ['time', 'thru', 'iso']
        dataUnits = ['months', 'facID', 'tons']
        
        call

        q.riverPlot(streamDim = 'iso', selectDim = 'thru', selectItem = 5)
        """

        if not self.isExecuted :
            raise QueryException, "Error: plotting can only be " + \
                         "performed after Query execution."

        if selectDim == streamDim :
            raise QueryException, "Error, streamDim and selectDim values were the " + \
                        "same. To plot only a single stream when streamDim has more " + \
                        "than one element, use a single-item streamList."

        # Parse the dimensions.
        try :
             timeDim = self.dataAxes.index('time')
             streamDim = self.dataAxes.index(streamDim)
             if selectDim != None :
                 selectDim = self.dataAxes.index(selectDim)

        except ValueError :
             raise QueryException, "Warning: Query data no longer have the requested" +\
                         "dimension (and the 'time' dimension, without which it's not" + \
                         "meaningful to create a river plot." 

        # Make sure the we don't have too many data dimensions.
        if len(self.data.shape) > 3 :
            raise QueryException, "Warning: data dimensionality too large. You " + \
                         "can't do a river plot of data that's larger than 3D--two " + \
                         "dimensions that you plot and one dimenion that you choose an " + \
                         "item from."

        # If they gave no streamlist, assume they want all possible streams.
        if None == streamList :
             streamList = self.dataLabels[streamDim]

        # Let's create a new view of the data to plot...
        plotData = self.data
        
        # And reduce it if that's what we've been told to do.
        if None != selectDim :
            if None == selectItem :
                raise QueryException, "If you specify a selectDim, you must specify " + \
                            "the label of the item you want to select."
            selectInd = self.dataLabels[selectDim].index(selectItem)
            if 1 == selectDim and 2 == streamDim :
                plotData = plotData[:, selectInd, :]
            elif 2 == selectDim and 1 == streamDim :
                plotData = plotData[:, :, selectInd]
            else :
                raise QueryException, "Error: bad function input or the data axes" + \
                            "have gotten out of order somehow."

        # Now we should be down to two dimensions. Check.
        if len(plotData.shape) != 2 :
            raise QueryException, "Error: bad streamDim/selectDim combo. You can " + \
                        "only make a river plot of a 2D data array."

        # Creae the figure and the data we need to do the plotting.
        fig = pylab.figure(1) # the figure
        ax = fig.add_subplot(111) # the axes
        t = self.dataLabels[timeDim] # get time dimension labels
        runSum = zeros(self.data.shape[timeDim])
        graphLim = 0
        colors = get_colours(len(streamList))
        # For RANDOM colors:
        # colors = pylab.rand(len(streamList),len(streamList))

        # Turn the list of stream labels into a list of indices.
        indList = [0] * len(streamList)
        for i, s in enumerate(streamList) :
             indList[i] = self.dataLabels[streamDim].index(s)
        
        # Iterate through the streams and add them to the plot.
        for ind in indList :
             ax.fill_between(t, runSum, runSum + plotData[:,ind], \
                                              facecolor=colors[ind],alpha=0.9, \
                                             label= str(ind))
             runSum += plotData[:,ind]
        
        # Override the default x-axis behavior.
        ax.set_xlim(xmin = self.t0, xmax = self.tf)

        # Use a reasonable scale on the y axis
        #graphLim = max(runSum)*1.05
        graphLim = mean(runSum)
        ax.set_ylim(ymin=0.01, ymax= graphLim )
        #ax.set_ylim(ymax=)

        # Use a log scale if plotting by isotope.
        #if self.dataAxes[streamDim] == 'iso' :
        #    ax.set_yscale('log')
        #    ax.set_ylim(ymin=1e-6)



        # Assign the figure to the appropriate Query class data member.
        self.figure = fig

###############################################################################

    def savePlot(self, filename = '') :
        """
        Saves the figure currently stored in this Query object, or throws a
        QueryException if none exists. The argument is the desired filename.
        """

        if None == self.figure :
            raise QueryException, "Error: this Query hasn't been asked to plot " + \
                        "anything."

        if '' == filename :
            raise QueryException, "Error: please give filename for plot output."

        fig = self.figure
        pylab.savefig(filename)

###############################################################################
###############################################################################
class SqlStmt(object) :
    """
    A class for storing the text of SQL statements to be executed.
    """

    sStr = ''
    fStr = ''
    wStr = ''
    oStr = ''

###############################################################################

    def __init__(self, s, f, w = None, o = None) :
        """
        Initializes the SELECT, FROM, and (optionally) WHERE clauses of the SqlStmt.
        Do not include the keywords themselves in the clauses, but do separate items
        by commas and use standard syntax.
        """

        self.sStr = 'SELECT ' + s + ' '
        self.fStr = 'FROM ' + f + ' '
        if None != w :
            self.wStr = 'WHERE ' + w
        if None != o :
            self.oStr = 'ORDER BY' + o

###############################################################################

    def __str__(self) :
        return self.sStr + self.fStr + self.wStr + self.oStr

###############################################################################

    def replaceSelect(self, columns) :

        self.sStr = 'SELECT' + columns + ' '

###############################################################################

    def replaceFrom(self, tables) :

        self.fStr = 'FROM' + tables + ' '

###############################################################################

    def replaceWhere(self, conditions) :

        self.wStr = 'WHERE' + conditions + ' '

###############################################################################

    def replaceOrder(self, sorting) :

        self.oStr = 'ORDER BY ' + sorting + ' '

###############################################################################
###############################################################################

class QueryException(Exception) :
    """
    An exception class for use by the Query class in the GENIUS GenPost module
    """

    def __init__(self, value) :
        """Creates a new QueryException"""
        self.value = value
    def __str__(self) :
        """Returns a string representation of this QueryException"""
        return repr(self.value)

###############################################################################
###############################################################################

def getIsoList() :
    return [
        20040,
        882260, 
        882280, 
        822060, 
        822070, 
        822080,     
        822100,     
        902280,         
        902290,            
        902300,            
        902320,            
        832090,
        892270,
        912310,
        922320,
        922330,
        922340,
        922350,
        922360,
        922380,
        932370,
        942380,
        942390,
        942400,
        942410,
        942420,
        942440,
        952410,
        952421,
        952430,
        962420,
        962430,
        962440,
        962450,
        962460,
        962470,
        962480,
        962500,
        982490,
        982500,
        982510,
        982520,
        10030,
        60140,
        60129,
        360810,
        360850,
        360849,
        380900,
        380889,
        430990,
        430989,
        430970,
        531290,
        531279,
        551340,
        551350,
        551370,
        551339,
        9992409,
        8881159
        ]


