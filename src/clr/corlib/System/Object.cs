namespace /*Genos.*/System
{
  public class Object
  {
    private string _typeName;

    public Object ()
    {
      _typeName = "System.Object";
    }

    ~Object ()
    {
    }

    public static bool ReferenceEquals (object objA, object objB)
    {
      return (objA == objB);
    }

    public virtual string ToString ()
    {
      return _typeName;
    }
  }
}
