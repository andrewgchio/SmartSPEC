package modelutils;

import model.SpacesModel.Space;

/**
 * A utility class to keep track of the three types of coordinates used in the
 * SmartSPEC GUI: user coordinates, pixel coordinates, and node coordinates.
 */
public class CoordinateManager {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance variables
    ////////////////////////////////////////////////////////////////////////////

    /**
     * User coordinates representing top left (x0,y0) and bottom right (x1, y1)
     */
    public double x0, y0, x1, y1;

    /**
     * Width/Height of the window
     */
    public double windowWidth, windowHeight;

    /**
     * Width/Height of a backdrop image
     */
    public double imageWidth, imageHeight;

    /**
     * The offset in X and Y in drawing the image
     */
    public double imageOffsetX, imageOffsetY;

    /**
     * The xy radii of nodes (in user coordinates)
     */
    public double[] Rxy = new double[2];

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Setter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Cache the size of the space backdrop image
     *
     * @param width  The width of the image
     * @param height The height of the image
     */
    public void setImageSize(double width, double height) {
        imageWidth = width;
        imageHeight = height;
    }

    /**
     * Set the specified user coordinates
     *
     * @param x0 The x-coordinate of top left corner
     * @param y0 The y-coordinate of top left corner
     * @param x1 The x-coordinate of bottom right corner
     * @param y1 The y-coordinate of bottom right corner
     */
    public void setUserCoordinates(double x0, double y0, double x1, double y1) {
        this.x0 = x0;
        this.y0 = y0;
        this.x1 = x1;
        this.y1 = y1;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Other Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Compute utility values based on the provided window width/height
     *
     * @param width  The width of the enclosing window
     * @param height The height of the enclosing window
     * @param R      The radius of the node
     */
    public void computeUtilityValues(double width, double height, int R) {
        windowWidth  = width;
        windowHeight = height;

        imageOffsetX = (windowWidth - imageWidth) / 2;
        imageOffsetY = (windowHeight - imageHeight) / 2;

        Rxy[0]       = R / imageWidth * (x1 - x0);
        Rxy[1]       = R / imageHeight * (y1 - y0);
    }

    /**
     * Return the user coordinates corresponding to pixel coordinates
     *
     * @param x The x pixel coordinate
     * @param y The y pixel coordinate
     * @return The translated xy user coordinates
     */
    public double[] pixelToUser(double x, double y) {
        return new double[] {
                (x - imageOffsetX) / imageWidth  * (x1 - x0) + x0,
                (y - imageOffsetY) / imageHeight * (y1 - y0) + y0 };
    }

    /**
     * Return the user coordinates corresponding to pixel coordinates
     *
     * @param xy The xy pixel coordinates
     * @return The translated xy user coordinates
     */
    public double[] pixelToUser(double[] xy) {
        return pixelToUser(xy[0], xy[1]);
    }

    /**
     * Return the pixel coordinates corresponding to user coordinates
     *
     * @param x The x user coordinate
     * @param y The y user coordinate
     * @return The translated xy pixel coordinates
     */
    public double[] userToPixel(double x, double y) {
        return new double[] {
                (x - x0) / (x1 - x0) * imageWidth  + imageOffsetX,
                (y - y0) / (y1 - y0) * imageHeight + imageOffsetY };
    }

    /**
     * Return the pixel coordinates corresponding to user coordinates
     *
     * @param xy The xy user coordinates
     * @return The translated xy pixel coordinates
     */
    public double[] userToPixel(double[] xy) {
        return userToPixel(xy[0], xy[1]);
    }

    /**
     * Return the pixel coordinates corresponding to node coordinates
     *
     * @param x The x node coordinate (represented in pixel coordinate plane)
     * @param y The y node coordinate (represented in pixel coordinate plane)
     * @param c The space represented by the node
     * @return The translated xy pixel coordinates
     */
    public double[] nodeToPixel(double x, double y, Space c) {
        double[] xyPixel = userToPixel(c.getX(), c.getY());
        return new double[] {
                x + xyPixel[0] - Rxy[0],
                y + xyPixel[1] - Rxy[1] };
    }

    /**
     * Return the pixel coordinates corresponding to node coordinates
     *
     * @param xy The xy node coordinates (represented in pixel coordinate plane)
     * @param c  The space represented by the node
     * @return The translated xy pixel coordinates
     */
    public double[] nodeToPixel(double[] xy, Space c) {
        return nodeToPixel(xy[0], xy[1], c);
    }

    /**
     * Return given coordinates after rounding
     *
     * @param x The x-coordinate to round
     * @param y The y-coordinate to round
     * @return The rounded xy coordinates
     */
    public double[] roundCoordinates(double x, double y) {
        return new double[] {
                Math.round(x * DoubleKeyMap.ROUND) / DoubleKeyMap.ROUND,
                Math.round(y * DoubleKeyMap.ROUND) / DoubleKeyMap.ROUND, };
    }

    /**
     * Return given coordinates after rounding
     *
     * @param xy The xy-coordinates to round
     * @return The rounded xy coordinates
     */
    public double[] roundCoordinates(double[] xy) {
        return roundCoordinates(xy[0], xy[1]);
    }

    /**
     * Return true if the given xy user coordinates are contained in the
     * specified user coordinates
     *
     * @param x The query x-coordinate
     * @param y The query y-coordinate
     * @return True if the given xy coordinates are contained in the specified
     *         user coordinates
     */
    public boolean containsUserCoordinates(double x, double y) {
        return x0 <= x && x <= x1 && y0 <= y && y <= y1;
    }

    /**
     * Return true if the given xy user coordinates are contained in the 
     * specified user coordinates
     *
     * @param xy The query xy-coordinates
     * @return True if the given xy coordinates is contained in the specified 
     *         user coordinates
     */
    public boolean containsUserCoordinates(double[] xy) {
        return containsUserCoordinates(xy[0], xy[1]);
    }

}
