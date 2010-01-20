namespace System.Reflection
{
  public sealed class DefaultMemberAttribute : Attribute
  {
    private string _memberName;

    public DefaultMemberAttribute (string memberName)
    {
      _memberName = memberName;
    }

    public string MemberName { get { return _memberName; } }
  }
}
