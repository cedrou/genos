namespace System
{
  public abstract class MulticastDelegate : Delegate
  {
    private IntPtr _method;

    public MulticastDelegate ()
    {
    }

    ~MulticastDelegate ()
    {
    }
  }
}
