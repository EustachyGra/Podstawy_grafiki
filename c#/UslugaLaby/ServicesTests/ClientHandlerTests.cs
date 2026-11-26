using Moq;
using ServerLibrary;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Xunit;

public class ClientHandlerUnitTests
{
    private ClientHandler CreateHandler(Mock<IDatabaseManager> dbMock, Mock<ISystemLogger> loggerMock)
    {
        return new ClientHandler(dbMock.Object, loggerMock.Object);
    }

    [Fact]
    public async Task LoginUser_WhenDbTrue_Returns_LOGIN_SUCCESS()
    {
        var db = new Mock<IDatabaseManager>();
        var logger = new Mock<ISystemLogger>();
        db.Setup(d => d.LoginUserAsync("alice", "pwd")).ReturnsAsync(true);

        var handler = CreateHandler(db, logger);
        var res = await handler.LoginUser("alice", "pwd");

        Assert.Equal("LOGIN_SUCCESS", res);
    }

    [Fact]
    public async Task LoginUser_WhenDbFalse_Returns_LOGIN_FAILED()
    {
        var db = new Mock<IDatabaseManager>();
        var logger = new Mock<ISystemLogger>();
        db.Setup(d => d.LoginUserAsync("bob", "bad")).ReturnsAsync(false);

        var handler = CreateHandler(db, logger);
        var res = await handler.LoginUser("bob", "bad");

        Assert.Contains("LOGIN_FAILED", res);
    }

    [Fact]
    public async Task RegisterUser_Valid_Returns_REGISTER_SUCCESS()
    {
        var db = new Mock<IDatabaseManager>();
        var logger = new Mock<ISystemLogger>();
        db.Setup(d => d.RegisterUserAsync("u1", "p1")).ReturnsAsync(true);

        var handler = CreateHandler(db, logger);
        var res = await handler.RegisterUser("u1", "p1");

        Assert.Equal("REGISTER_SUCCESS", res);
    }

    [Fact]
    public async Task RegisterUser_EmptyUsername_ThrowsArgumentNullException()
    {
        var db = new Mock<IDatabaseManager>();
        var logger = new Mock<ISystemLogger>();

        var handler = CreateHandler(db, logger);

        await Assert.ThrowsAsync<ArgumentNullException>(() => handler.RegisterUser("", "p"));
    }

}
