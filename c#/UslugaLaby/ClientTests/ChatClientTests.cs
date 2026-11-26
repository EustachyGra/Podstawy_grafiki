using Moq;
using System;
using System.Threading.Tasks;
using Xunit;

namespace Client.Tests
{
    public class ChatClientTests
    {
        private ChatClient CreateClient(Mock<IChatTransport> transport, Mock<ISystemLogger> logger)
        {
            return new ChatClient(transport.Object, logger.Object);
        }

        [Fact]
        public async Task SendCommandAsync_NullCommand_ThrowsArgumentNullException()
        {
            Mock<IChatTransport> transportMock = new Mock<IChatTransport>();
            Mock<ISystemLogger> loggerMock = new Mock<ISystemLogger>();
            ChatClient client = CreateClient(transportMock, loggerMock);
            await Assert.ThrowsAsync<ArgumentNullException>(
                () => client.SendCommandAsync(null)
            );
        }
        [Fact]
        public async Task SendCommandAsync_NotConnected_CallsConnectAsync()
        {
            Mock<IChatTransport> transportMock = new Mock<IChatTransport>();
            Mock<ISystemLogger> loggerMock = new Mock<ISystemLogger>();
            ChatClient client = CreateClient(transportMock, loggerMock);
            transportMock.Setup(t => t.IsConnected).Returns(false);
            transportMock.Setup(t => t.ConnectAsync())
                         .Returns(Task.CompletedTask);
            transportMock.Setup(t => t.WriteLineAsync("CMD"))
                         .Returns(Task.CompletedTask);
            transportMock.Setup(t => t.ReadLineAsync())
                         .ReturnsAsync("OK");

            string[] result = await client.SendCommandAsync("CMD");

            transportMock.Verify(
                t => t.ConnectAsync(),
                Times.Once
            );
        }
        [Fact]
        public async Task SendCommandAsync_NoTerminator_ReadsSingleLine()
        {
            Mock<IChatTransport> transportMock = new Mock<IChatTransport>();
            Mock<ISystemLogger> loggerMock = new Mock<ISystemLogger>();
            ChatClient client = CreateClient(transportMock, loggerMock);
            transportMock.Setup(t => t.IsConnected).Returns(true);
            transportMock.Setup(t => t.WriteLineAsync("PING"))
                         .Returns(Task.CompletedTask);
            transportMock.Setup(t => t.ReadLineAsync())
                         .ReturnsAsync("PONG");

            string[] result = await client.SendCommandAsync("PING");

            Assert.Single(result);
            Assert.Equal("PONG", result[0]);
        }
        [Fact]
        public async Task SendCommandAsync_WithTerminator_ReadsUntilTerminator()
        {
            Mock<IChatTransport> transportMock = new Mock<IChatTransport>();
            Mock<ISystemLogger> loggerMock = new Mock<ISystemLogger>();
            ChatClient client = CreateClient(transportMock, loggerMock);
            transportMock.Setup(t => t.IsConnected).Returns(true);
            transportMock.Setup(t => t.WriteLineAsync("LIST"))
                         .Returns(Task.CompletedTask);

            transportMock.SetupSequence(t => t.ReadLineAsync())
                .ReturnsAsync("A")
                .ReturnsAsync("B")
                .ReturnsAsync("END");

            string[] result = await client.SendCommandAsync("LIST", "END");

            Assert.Equal(2, result.Length);
            Assert.Equal("A", result[0]);
            Assert.Equal("B", result[1]);
        }
        [Fact]
        public async Task SendCommandAsync_DoesNotIncludeTerminatorInResult()
        {
            Mock<IChatTransport> transportMock = new Mock<IChatTransport>();
            Mock<ISystemLogger> loggerMock = new Mock<ISystemLogger>();
            ChatClient client = CreateClient(transportMock, loggerMock);
            transportMock.Setup(t => t.IsConnected).Returns(true);
            transportMock.Setup(t => t.WriteLineAsync(It.IsAny<string>()))
                         .Returns(Task.CompletedTask);

            transportMock.SetupSequence(t => t.ReadLineAsync())
                .ReturnsAsync("LINE1")
                .ReturnsAsync("STOP");

            string[] result = await client.SendCommandAsync("CMD", "STOP");

            Assert.DoesNotContain("STOP", result);
        }

    }
}