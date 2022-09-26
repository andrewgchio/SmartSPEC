package model;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.stream.Stream;

import modelutils.ErrorStringBuilder;
import view.View;

public abstract class EntityModel<T extends EntityModel<T>.Entity> 
implements Iterable<T> {
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    /**
     * A map of entity IDs to entities
     */
    protected Map<Integer, T> idMap = new HashMap<>();
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Entity Class Definition
    ////////////////////////////////////////////////////////////////////////////

    /**
     * An abstract class from which all entity classes should derive
     */
    public abstract class Entity {

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Instance Variables
        ////////////////////////////////////////////////////////////////////////

        /**
         * The ID of the entity
         */
        protected Integer id = null;

        /**
         * The semantic description of the entity
         */
        protected String description = "";

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Getter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return the ID of the entity
         * 
         * @return The ID of the entity
         */
        public Integer getID() {
            return id;
        }

        /**
         * Return the semantic description of the entity
         * 
         * @return The semantic description of the entity
         */
        public String getDescription() {
            return description;
        }

        /**
         * Return a display string for the entity
         * 
         * @return A display string for the entity
         */
        public String getDisplayName() {
            return id + " (" + description + ")";
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Setter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Set the ID of the entity
         * 
         * @param id The ID of the entity
         */
        public void setID(Integer id) {
            this.id = id;
        }

        /**
         * Set the semantic description of the entity
         * 
         * @param description The semantic description of the entity
         */
        public void setDescription(String description) {
            this.description = description;
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Other Methods
        ////////////////////////////////////////////////////////////////////////
        
        /**
         * Return true if the entity is valid
         * @return True if the entity is valid
         */
        public abstract boolean isValid();

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Iterators / Streams
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return an iterator over the entities
     * 
     * @return An iterator over the entities
     */
    @Override
    public Iterator<T> iterator() {
        return idMap.values().iterator();
    }

    /**
     * Return a stream over the entities
     * 
     * @return A stream over the entities
     */
    public Stream<T> stream() {
        return idMap.values().stream();
    }
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return true if there is an entity with the given ID
     * 
     * @param id The ID to check
     * @return True if there is an entity with the given ID
     */
    public boolean containsID(Integer id) {
        return idMap.containsKey(id);
    }

    /**
     * Return the entity corresponding to the given ID
     * 
     * @param id The entity ID
     * @return The entity corresponding to the given ID
     */
    public T get(Integer id) {
        return idMap.get(id);
    }

    /**
     * Add an entity to the model
     * 
     * @param t The entity to add
     */
    public void add(T t) {
        idMap.put(t.id, t);
    }

    /**
     * Remove an entity from the model
     * 
     * @param t The entity to remove
     */
    public abstract void remove(T t);
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Other Methods
    ////////////////////////////////////////////////////////////////////////////
    
    /**
     * Check whether all entities stored in the model are valid
     * @param print Set to true to print out errors
     * @return True if all entities stored in the model are valid
     */
    public boolean checkValid(boolean print) {
        String classname = Entity.class.getSimpleName();

        ErrorStringBuilder err = new ErrorStringBuilder();
        for (EntityModel<T>.Entity ent : this) {
            if (!ent.isValid())
                err.append("Invalid " + classname, ent.getDisplayName());
        }
        
        if (print && !err.isEmpty())
            View.log(err.toString());
        
        // if empty, no errors were found
        return err.isEmpty();
    }

}
